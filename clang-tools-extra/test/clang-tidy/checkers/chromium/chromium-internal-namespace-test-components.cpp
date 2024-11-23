// RUN: %check_clang_tidy %s chromium-internal-namespace %t -- \
// RUN:   -config='{CheckOptions: [{key: chromium-internal-namespace.test_source_path, value: "/src/components/autofill/foo.cc"}]}'

namespace autofill {
namespace internal {
void bar() {}
}  // namespace internal
}  // namespace autofill

void test_usage() {
  autofill::internal::bar();  // No warning - we're in components/autofill/
}