// RUN: %check_clang_tidy %s chromium-internal-namespace %t

namespace base::internal {
struct Helper {};
}

namespace chrome {
void feature() {
  // CHECK-MESSAGES: :[[@LINE+1]]:3: warning: using internal namespace 'base::internal::' from outside its owning directory
  base::internal::Helper h;
}
}
