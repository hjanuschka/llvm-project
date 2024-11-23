#include "BlinkCastCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"
#include "llvm/Support/Path.h"

using namespace clang::ast_matchers;

namespace clang::tidy::chromium {

namespace {
bool isInBlinkNamespace(const Decl* D) {
  const DeclContext* Context = D->getDeclContext();
  while (Context) {
    if (const auto* NS = dyn_cast<NamespaceDecl>(Context)) {
      if (NS->getName() == "blink") {
        return true;
      }
    }
    Context = Context->getParent();
  }
  return false;
}

bool isInBlinkDirectory(const SourceLocation& Loc, const SourceManager& SM) {
  if (Loc.isInvalid()) {
    return false;
  }

  FileID FID = SM.getFileID(Loc);
  const FileEntry* Entry = SM.getFileEntryForID(FID);
  if (!Entry) {
    return false;
  }

  StringRef Path = SM.getFilename(Loc);
  return Path.contains("third_party/blink/") || 
         Path.contains("third_party\\blink\\") ||
         Path.contains("/blink/") ||
         Path.contains("\\blink\\");
}

std::string getSourceText(const SourceRange& Range, const ASTContext& Context) {
  const SourceManager &SM = Context.getSourceManager();
  const LangOptions &LangOpts = Context.getLangOpts();
  CharSourceRange CharRange = CharSourceRange::getTokenRange(Range);
  return Lexer::getSourceText(CharRange, SM, LangOpts).str();
}
}  // namespace

void ChromiumBlinkCastCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      cxxStaticCastExpr(
        hasAncestor(functionDecl().bind("func"))
      ).bind("static_cast"),
      this);
}

void ChromiumBlinkCastCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Cast = Result.Nodes.getNodeAs<CXXStaticCastExpr>("static_cast");
  const auto *Func = Result.Nodes.getNodeAs<FunctionDecl>("func");
  if (!Cast || !Func)
    return;

  bool inBlinkCode = isInBlinkNamespace(Func) || 
                    isInBlinkDirectory(Func->getLocation(), *Result.SourceManager);
  
  if (!inBlinkCode)
    return;

  // Get the target type's CXXRecordDecl if it's a class type
  const CXXRecordDecl* RecordDecl = nullptr;
  if (const auto* PointeeType = Cast->getType()->getPointeeType().getTypePtr()) {
    if (auto* RT = PointeeType->getAs<RecordType>()) {
      RecordDecl = dyn_cast<CXXRecordDecl>(RT->getDecl());
    }
  }
  
  if (!RecordDecl)
    return;

  bool IsPoly = RecordDecl->isPolymorphic();
  
  std::string SubExprStr = getSourceText(Cast->getSubExpr()->getSourceRange(), *Result.Context);
  std::string TypeStr = getSourceText(Cast->getTypeInfoAsWritten()->getTypeLoc().getSourceRange(), *Result.Context);
  
  // Remove pointer star if present
  if (TypeStr.back() == '*') {
    TypeStr.pop_back();
  }
  // Trim any whitespace
  TypeStr.erase(TypeStr.find_last_not_of(" \t") + 1);

  // Use DynamicTo for polymorphic types, To for non-polymorphic
  std::string Replacement = IsPoly ? "DynamicTo" : "To";
  Replacement += "<" + TypeStr + ">(" + SubExprStr + ")";

  diag(Cast->getBeginLoc(),
       "static_cast in Blink should be replaced with %0")
      << (IsPoly ? "DynamicTo<T>" : "To<T>")
      << FixItHint::CreateReplacement(Cast->getSourceRange(), Replacement);
}

} // namespace clang::tidy::chromium