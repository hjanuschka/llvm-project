// RUN: %check_clang_tidy %s chromium-internal-namespace %t -- \
// RUN:   -config='{CheckOptions: [{key: chromium-internal-namespace.test_source_path, value: "/out/Debug/gen/base/foo.cc"}]}'

namespace base {
namespace internal {
void foo() {}
}  // namespace internal
}  // namespace base

void test_usage() {
  base::internal::foo();  // No warning - normalized path is in base/
}