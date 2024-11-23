// RUN: %check_clang_tidy %s chromium-internal-namespace %t -- \
// RUN:   -config='{CheckOptions: [{key: chromium-internal-namespace.test_source_path, value: "/src/chrome/browser/internal/test.cc"}]}'

namespace base {
namespace internal {
void foo() {}
}  // namespace internal
}  // namespace base

namespace chrome {
namespace browser {
namespace internal {
void test_chrome_internal() {
  base::internal::foo();
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: do not use base::internal namespace from outside base/ directory [chromium-internal-namespace]
}
}  // namespace internal
}  // namespace browser
}  // namespace chrome