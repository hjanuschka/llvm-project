// RUN: %check_clang_tidy %s chromium-blink-cast %t

namespace blink {

class Base {
public:
  virtual ~Base() = default;
};

class Derived : public Base {
public:
  void derivedMethod() {}
};

void test() {
  Base* base = new Base();
  
  // Should suggest DynamicTo for polymorphic types
  auto* d1 = static_cast<Derived*>(base);
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: use DynamicTo<Derived> instead of static_cast in Blink code
  // CHECK-FIXES: auto* d1 = DynamicTo<Derived>(base);
}

} // namespace blink
