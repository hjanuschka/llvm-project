#include "InternalNamespaceCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/Support/Path.h"

using namespace clang::ast_matchers;

namespace clang::tidy::chromium {

void ChromiumInternalNamespaceCheck::registerMatchers(MatchFinder *Finder) {
  // Match any usage of a nested-name-specifier that ends with "internal::"
  Finder->addMatcher(
      nestedNameSpecifierLoc(
          loc(specifiesNamespace(hasName("::internal"))))
          .bind("internal_ns"),
      this);
}

void ChromiumInternalNamespaceCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *NNS = Result.Nodes.getNodeAs<NestedNameSpecifierLoc>("internal_ns");
  if (!NNS)
    return;

  // Get the location of the 'internal' namespace usage
  SourceLocation Loc = NNS->getLocalBeginLoc();

  // Get the source file path
  const SourceManager &SM = *Result.SourceManager;
  StringRef File = SM.getFilename(Loc);

  // Check if the file is in an internal directory
  StringRef Parent = llvm::sys::path::parent_path(File);
  if (!Parent.ends_with("internal")) {
    diag(Loc, "do not use internal namespaces from outside internal directories");
  }
}

} // namespace clang::tidy::chromium