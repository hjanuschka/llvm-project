// RUN: %check_clang_tidy %s chromium-time-unit %t

constexpr int kTimeoutMs = 5000;
// CHECK-MESSAGES: :[[@LINE-1]]:15: warning: prefer using base::Milliseconds for time constants instead of raw integers [chromium-time-unit]
// CHECK-FIXES: constexpr base::TimeDelta kTimeoutMs = base::Milliseconds(5000)

constexpr int kDelaySeconds = 30;
// CHECK-MESSAGES: :[[@LINE-1]]:15: warning: prefer using base::Seconds for time constants instead of raw integers [chromium-time-unit]
// CHECK-FIXES: constexpr base::TimeDelta kDelaySeconds = base::Seconds(30)

// These should not trigger warnings
constexpr int kBufferSize = 1024;
constexpr int kMaxRetries = 3;