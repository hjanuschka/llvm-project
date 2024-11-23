#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_TIMEUNITCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_TIMEUNITCHECK_H

#include "../ClangTidyCheck.h"
#include <unordered_map>

namespace clang::tidy::chromium {

/// Checks for raw integers used as time values and suggests using appropriate time unit types.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/chromium/time-unit.html
class ChromiumTimeUnitCheck : public ClangTidyCheck {
public:
  ChromiumTimeUnitCheck(StringRef Name, ClangTidyContext *Context);
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  const bool UseChromiumStyle;
  bool isTimeRelatedName(const std::string &Name, std::string &TimeFunction);
  std::string getNamespace() const;
  std::string buildReplacement(const std::string &VarName, 
                             const std::string &TimeFunction,
                             const std::string &Value);

  const std::unordered_map<std::string, std::pair<std::string, std::string>> kTimeUnitPatterns;
};

} // namespace clang::tidy::chromium

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CHROMIUM_TIMEUNITCHECK_H
