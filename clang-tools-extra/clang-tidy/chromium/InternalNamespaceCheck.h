#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_INTERNALNAMESPACECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_INTERNALNAMESPACECHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::chromium {

/// Checks for usage of internal namespaces outside their intended scope.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/chromium/internal-namespace.html
class ChromiumInternalNamespaceCheck : public ClangTidyCheck {
public:
  ChromiumInternalNamespaceCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::chromium

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_INTERNALNAMESPACECHECK_H
