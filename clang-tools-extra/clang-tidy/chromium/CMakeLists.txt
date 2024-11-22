set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  Support
  )

 add_clang_library(clangTidyChromiumModule STATIC
  TimeUnitCheck.cpp
  InternalNamespaceCheck.cpp
  BlinkCastCheck.cpp
  ChromiumModule.cpp
  LINK_LIBS
  clangTidy
  clangTidyReadabilityModule
  clangTidyUtils

  DEPENDS
  omp_gen
  ClangDriverOptions
  )

clang_target_link_libraries(clangTidyChromiumModule
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangTooling
  )
