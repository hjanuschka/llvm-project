#include "TimeUnitCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <regex>

using namespace clang::ast_matchers;

namespace clang::tidy::chromium {

ChromiumTimeUnitCheck::ChromiumTimeUnitCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context),
      UseChromiumStyle(Options.get("use-chromium-style", true)),
      kTimeUnitPatterns({
          {"Ms$|MilliSeconds?$|InMs$", {"milliseconds", "Milliseconds"}},
          {"Us$|MicroSeconds?$|InUs$", {"microseconds", "Microseconds"}},
          {"Ns$|NanoSeconds?$|InNs$", {"nanoseconds", "Nanoseconds"}},
          {"Seconds?$|InSeconds?$", {"seconds", "Seconds"}},
          {"Minutes?$|InMinutes?$", {"minutes", "Minutes"}},
          {"Hours?$|InHours?$", {"hours", "Hours"}}
      }) {}

void ChromiumTimeUnitCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "use-chromium-style", UseChromiumStyle);
}

void ChromiumTimeUnitCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      varDecl(
          anyOf(
              isConstexpr(),
              hasStaticStorageDuration()
          ),
          hasType(isInteger()),
          hasInitializer(integerLiteral().bind("literal")))
          .bind("var"),
      this);
}

bool ChromiumTimeUnitCheck::isTimeRelatedName(const std::string &Name, 
                                            std::string &TimeFunction) {
  for (const auto &[Pattern, Functions] : kTimeUnitPatterns) {
    std::regex TimePattern(Pattern, std::regex::icase);
    if (std::regex_search(Name, TimePattern)) {
      TimeFunction = UseChromiumStyle ? Functions.second : Functions.first;
      return true;
    }
  }
  return false;
}

std::string ChromiumTimeUnitCheck::getNamespace() const {
  return UseChromiumStyle ? "base" : "std::chrono";
}

std::string ChromiumTimeUnitCheck::buildReplacement(const std::string &VarName,
                                                  const std::string &TimeFunction,
                                                  const std::string &Value) {
  if (UseChromiumStyle) {
    return "constexpr base::TimeDelta " + VarName + " = base::" + 
           TimeFunction + "(" + Value + ")";
  } else {
    // For std::chrono, use the appropriate duration type
    std::string durationType;
    if (TimeFunction == "hours") {
      durationType = "hours";
    } else if (TimeFunction == "minutes") {
      durationType = "minutes";
    } else if (TimeFunction == "seconds") {
      durationType = "seconds";
    } else if (TimeFunction == "milliseconds") {
      durationType = "milliseconds";
    } else if (TimeFunction == "microseconds") {
      durationType = "microseconds";
    } else if (TimeFunction == "nanoseconds") {
      durationType = "nanoseconds";
    }
    
    return "constexpr std::chrono::" + durationType + " " + VarName + 
           " = std::chrono::" + TimeFunction + "(" + Value + ")";
  }
}

void ChromiumTimeUnitCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *VD = Result.Nodes.getNodeAs<VarDecl>("var");
  const auto *Literal = Result.Nodes.getNodeAs<IntegerLiteral>("literal");

  if (!VD || !Literal)
    return;

  std::string TimeFunction;
  std::string VarName = VD->getNameAsString();
  
  if (!isTimeRelatedName(VarName, TimeFunction))
    return;

  const auto Value = Literal->getValue().getLimitedValue();
  
  auto Replacement = buildReplacement(VarName, TimeFunction, std::to_string(Value));
  
  auto Hint = FixItHint::CreateReplacement(VD->getSourceRange(), Replacement);

  diag(VD->getLocation(),
       "prefer using %0::%1 for time constants instead of raw integers")
      << getNamespace() << TimeFunction << Hint;
}

} // namespace clang::tidy::chromium
