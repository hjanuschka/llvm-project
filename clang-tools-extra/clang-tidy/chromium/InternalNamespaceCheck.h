#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_INTERNAL_NAMESPACE_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_INTERNAL_NAMESPACE_CHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::chromium {

class ChromiumInternalNamespaceCheck : public ClangTidyCheck {
public:
  ChromiumInternalNamespaceCheck(StringRef Name, ClangTidyContext *Context);
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;

private:
  bool isInComponentPath(llvm::StringRef FilePath, llvm::StringRef ComponentName);
  std::string TestSourcePath;
};

} // namespace clang::tidy::chromium

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_INTERNAL_NAMESPACE_CHECK_H