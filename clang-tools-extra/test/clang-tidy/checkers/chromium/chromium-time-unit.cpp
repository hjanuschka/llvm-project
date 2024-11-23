// RUN: %check_clang_tidy %s chromium-time-unit %t

constexpr int kTimeoutMs = 5000;
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: prefer using std::chrono::milliseconds for time constants instead of raw integers
// CHECK-FIXES: constexpr auto kTimeoutMs = std::chrono::milliseconds(5000)

constexpr int kDelaySeconds = 30;
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: prefer using std::chrono::seconds for time constants instead of raw integers
// CHECK-FIXES: constexpr auto kDelaySeconds = std::chrono::seconds(30)
