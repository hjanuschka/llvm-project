#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_BLINKCASTCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_BLINKCASTCHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::chromium {

/// Checks for static_cast usage in Blink code and suggests using To<T> or DynamicTo<T>.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/chromium/blink-cast.html
class ChromiumBlinkCastCheck : public ClangTidyCheck {
public:
  ChromiumBlinkCastCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::chromium

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_BLINKCASTCHECK_H
