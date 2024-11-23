// RUN: %check_clang_tidy %s chromium-time-unit %t -- -config="{CheckOptions: {chromium-time-unit.use-chromium-style: 'false'}}"

constexpr int kTimeoutMs = 5000;
// CHECK-MESSAGES: :[[@LINE-1]]:15: warning: prefer using std::chrono::milliseconds for time constants instead of raw integers [chromium-time-unit]
// CHECK-FIXES: constexpr std::chrono::milliseconds kTimeoutMs = std::chrono::milliseconds(5000)

constexpr int kDelaySeconds = 30;
// CHECK-MESSAGES: :[[@LINE-1]]:15: warning: prefer using std::chrono::seconds for time constants instead of raw integers [chromium-time-unit]
// CHECK-FIXES: constexpr std::chrono::seconds kDelaySeconds = std::chrono::seconds(30)

// These should not trigger warnings
constexpr int kBufferSize = 1024;
constexpr int kMaxRetries = 3;