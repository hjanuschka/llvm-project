#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"
#include "BlinkCastCheck.h"
#include "InternalNamespaceCheck.h"
#include "TimeUnitCheck.h"

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

// Register the module
static clang::tidy::ClangTidyModuleRegistry::Add<ChromiumModule>
    X("chromium", "Adds Chromium-specific checks.");

} // namespace clang::tidy::chromium

namespace clang {
namespace tidy {

// This anchor is used to force the linker to link the ChromiumModule.
volatile int ChromiumModuleAnchorSource = 0;

} // namespace tidy
} // namespace clang
