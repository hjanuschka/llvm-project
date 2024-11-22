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
  const auto *NameLoc = Result.Nodes.getNodeAs<NestedNameSpecifierLoc>("internal_ns");
  if (!NameLoc)
    return;

  // Get the full namespace name
  std::string FullName;
  llvm::raw_string_ostream OS(FullName);
  NameLoc->getNestedNameSpecifier()->print(OS, Result.Context->getPrintingPolicy());

  // Extract the root namespace (everything before ::internal)
  StringRef NamespaceStr(FullName);
  size_t InternalPos = NamespaceStr.find("::internal");
  if (InternalPos == StringRef::npos)
    return;
  
  StringRef RootNamespace = NamespaceStr.substr(0, InternalPos);

  // Get current file's directory
  auto &SM = *Result.SourceManager;
  FileID FID = SM.getFileID(NameLoc->getBeginLoc());
  const FileEntry *File = SM.getFileEntryForID(FID);
  if (!File)
    return;

  StringRef CurrentPath = File->getName();
  StringRef CurrentDir = llvm::sys::path::parent_path(CurrentPath);
  
  // Convert root namespace to expected directory name
  // e.g., "base::foo" -> "base"
  StringRef ExpectedDir = RootNamespace.substr(0, RootNamespace.find(':'));
  
  // Check if current directory contains the namespace root
  if (!CurrentDir.contains(ExpectedDir)) {
    diag(NameLoc->getBeginLoc(),
         "using internal namespace '%0' from outside its owning directory")
        << FullName;
  }
}

} // namespace clang::tidy::chromium
