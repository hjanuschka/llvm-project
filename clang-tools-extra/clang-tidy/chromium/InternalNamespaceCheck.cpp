#include "InternalNamespaceCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/Support/Path.h"
#include <string>

using namespace clang::ast_matchers;

namespace clang::tidy::chromium {

ChromiumInternalNamespaceCheck::ChromiumInternalNamespaceCheck(
    StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {
  TestSourcePath = Options.get("test_source_path", "");
}

void ChromiumInternalNamespaceCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "test_source_path", TestSourcePath);
}

void ChromiumInternalNamespaceCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      nestedNameSpecifierLoc(
          loc(specifiesNamespace(hasName("internal"))))
          .bind("internal_ns"),
      this);
}

bool ChromiumInternalNamespaceCheck::isInComponentPath(StringRef FilePath, 
                                                     StringRef ComponentName) {
  SmallString<256> CanonicalPath(FilePath);
  llvm::sys::path::native(CanonicalPath, llvm::sys::path::Style::posix);
  StringRef Path(CanonicalPath);
  
  // Normalize build directory paths
  StringRef NormalizedPath = Path;
  if (Path.contains("/out/") || Path.contains("/Debug/") || Path.contains("/Release/")) {
    size_t GenPos = Path.find("/gen/");
    if (GenPos != StringRef::npos) {
      NormalizedPath = Path.substr(GenPos + 5); // Skip "/gen/"
    }
  }

  // Check each path component
  StringRef Remaining = NormalizedPath;
  while (!Remaining.empty()) {
    if (llvm::sys::path::filename(Remaining) == ComponentName) {
      StringRef Parent = llvm::sys::path::parent_path(Remaining);
      if (Parent.empty() || llvm::sys::path::filename(Parent) == "src" || 
          llvm::sys::path::filename(Parent) == "components") {
        return true;
      }
    }
    Remaining = llvm::sys::path::parent_path(Remaining);
  }
  
  return false;
}

void ChromiumInternalNamespaceCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *NNS = Result.Nodes.getNodeAs<NestedNameSpecifierLoc>("internal_ns");
  if (!NNS)
    return;

  SourceLocation Loc = NNS->getLocalBeginLoc();
  const SourceManager &SM = *Result.SourceManager;
  StringRef FilePath = !TestSourcePath.empty() ? TestSourcePath : SM.getFilename(Loc);
  if (FilePath.empty())
    return;

  std::string FullNamespace;
  for (NestedNameSpecifier *Prefix = NNS->getNestedNameSpecifier(); 
       Prefix; 
       Prefix = Prefix->getPrefix()) {
    if (const auto *NS = Prefix->getAsNamespace()) {
      if (!FullNamespace.empty()) {
        FullNamespace = "::" + FullNamespace;
      }
      FullNamespace = NS->getName().str() + FullNamespace;
    }
  }

  size_t InternalPos = FullNamespace.rfind("::internal");
  if (InternalPos == std::string::npos)
    return;

  std::string ComponentName;
  size_t LastSep = FullNamespace.rfind("::", InternalPos - 1);
  if (LastSep == std::string::npos) {
    ComponentName = FullNamespace.substr(0, InternalPos);
  } else {
    ComponentName = FullNamespace.substr(LastSep + 2, InternalPos - LastSep - 2);
  }

  if (!isInComponentPath(FilePath, ComponentName)) {
    diag(Loc, "do not use %0::internal namespace from outside %0/ directory")
        << ComponentName;
  }
}

} // namespace clang::tidy::chromium