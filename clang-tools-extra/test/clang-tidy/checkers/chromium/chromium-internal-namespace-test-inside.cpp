// RUN: %check_clang_tidy %s chromium-internal-namespace %t -- \
// RUN:   -config='{CheckOptions: [{key: chromium-internal-namespace.test_source_path, value: "/src/base/feature/test.cc"}]}'

namespace base {
namespace internal {
void foo() {}
}  // namespace internal
}  // namespace base

void test_usage() {
  base::internal::foo();  // No warning when in base/
}