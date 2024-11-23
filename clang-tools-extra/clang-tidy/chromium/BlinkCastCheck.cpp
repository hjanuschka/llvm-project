#include "BlinkCastCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang::tidy::chromium {

void ChromiumBlinkCastCheck::registerMatchers(MatchFinder *Finder) {
  // Match static_cast expressions
  Finder->addMatcher(
      cxxStaticCastExpr().bind("static_cast"),
      this);
}

// Helper function to get source text from a SourceRange
std::string getSourceText(const SourceRange& Range, const ASTContext& Context) {
  const SourceManager &SM = Context.getSourceManager();
  const LangOptions &LangOpts = Context.getLangOpts();
  
  // Create a CharSourceRange for the entire expression
  CharSourceRange CharRange = CharSourceRange::getTokenRange(Range);
  
  // Get the text from the source range
  return Lexer::getSourceText(CharRange, SM, LangOpts).str();
}

void ChromiumBlinkCastCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Cast = Result.Nodes.getNodeAs<CXXStaticCastExpr>("static_cast");
  if (!Cast)
    return;

  // Get the type of the expression being cast
  const Type* Type = Cast->getSubExpr()->getType().getTypePtr();
  
  // Get the CXXRecordDecl if this is a class type
  const CXXRecordDecl* RecordDecl = nullptr;
  if (auto* RT = Type->getAs<clang::RecordType>()) {
    RecordDecl = dyn_cast<CXXRecordDecl>(RT->getDecl());
  }
  
  bool CanUseDynamicCast = RecordDecl && RecordDecl->hasDefinition() && 
                          RecordDecl->isPolymorphic();

  if (CanUseDynamicCast) {
    // Get the source text for the subexpression and type
    std::string SubExprStr = getSourceText(Cast->getSubExpr()->getSourceRange(), *Result.Context);
    std::string TypeStr = getSourceText(Cast->getTypeInfoAsWritten()->getTypeLoc().getSourceRange(), *Result.Context);
    
    diag(Cast->getBeginLoc(),
         "static_cast to a polymorphic class hierarchy can be replaced with "
         "dynamic_cast")
        << FixItHint::CreateReplacement(
               Cast->getSourceRange(),
               "dynamic_cast<" + TypeStr + ">(" + SubExprStr + ")");
  }
}

} // namespace clang::tidy::chromium