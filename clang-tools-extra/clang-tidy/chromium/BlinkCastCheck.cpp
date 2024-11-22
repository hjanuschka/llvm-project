#include "BlinkCastCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace clang::ast_matchers;

namespace clang::tidy::chromium {

void ChromiumBlinkCastCheck::registerMatchers(MatchFinder *Finder) {
  // Match static_cast<T*> expressions
  Finder->addMatcher(
      cxxStaticCastExpr(
          // Match only pointer casts
          hasDestinationType(pointerType()),
          // Within Blink namespace
          hasAncestor(namespaceDecl(hasName("blink"))))
          .bind("cast"),
      this);
}

void ChromiumBlinkCastCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Cast = Result.Nodes.getNodeAs<CXXStaticCastExpr>("cast");
  
  // Get source location
  auto Loc = Cast->getBeginLoc();
  
  // Get target type without pointer
  QualType TargetType = Cast->getTypeAsWritten();
  if (const auto *PT = dyn_cast<PointerType>(TargetType)) {
    TargetType = PT->getPointeeType();
  }

  // Check if dynamic_cast would be valid
  bool CanUseDynamicCast = Cast->getSubExpr()->getType()->isPolymorphicClass();

  // Build replacement text
  std::string Replacement = (CanUseDynamicCast ? "DynamicTo<" : "To<") + 
                          TargetType.getAsString() + ">";

  // Report diagnostic and fix
  auto Diag = diag(Loc, "use %0 instead of static_cast in Blink code") 
              << Replacement;

  Diag << FixItHint::CreateReplacement(
      CharSourceRange::getTokenRange(Cast->getBeginLoc(), 
                                   Cast->getEndLoc()),
      Replacement + "(" + 
          tooling::getText(*Cast->getSubExpr(), *Result.Context) + ")");
}

} // namespace clang::tidy::chromium
