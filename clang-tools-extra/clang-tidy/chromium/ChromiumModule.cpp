#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"
#include "TimeUnitCheck.h"
#include "InternalNamespaceCheck.h"
#include "BlinkCastCheck.h"

namespace clang::tidy::chromium {

class ChromiumModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<ChromiumTimeUnitCheck>(
        "chromium-time-unit");
    CheckFactories.registerCheck<ChromiumInternalNamespaceCheck>(
        "chromium-internal-namespace");
    CheckFactories.registerCheck<ChromiumBlinkCastCheck>(
        "chromium-blink-cast");
  }
};

} // namespace clang::tidy::chromium

static clang::tidy::ClangTidyModuleRegistry::Add<clang::tidy::chromium::ChromiumModule>
    X("chromium", "Adds Chromium-specific checks.");
