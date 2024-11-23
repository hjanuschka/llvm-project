// RUN: %check_clang_tidy %s chromium-blink-cast %t

namespace blink {

class Base {
public:
    virtual void foo() {}
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    void foo() override {}
};

class NonPoly {
public:
    ~NonPoly() = default; // non-virtual
};

class DerivedNonPoly : public NonPoly {
};

void test() {
    Base* base = new Base();
    NonPoly* np = new NonPoly();
    
    // Should suggest DynamicTo for polymorphic types
    auto* d1 = static_cast<Derived*>(base);
    // CHECK-MESSAGES: :[[@LINE-1]]:16: warning: static_cast in Blink should be replaced with DynamicTo<T> [chromium-blink-cast]
    // CHECK-FIXES: auto* d1 = DynamicTo<Derived>(base);

    // Should suggest To for non-polymorphic types
    auto* d2 = static_cast<DerivedNonPoly*>(np);
    // CHECK-MESSAGES: :[[@LINE-1]]:16: warning: static_cast in Blink should be replaced with To<T> [chromium-blink-cast]
    // CHECK-FIXES: auto* d2 = To<DerivedNonPoly>(np);

    // Check const case
    const Base* const_base = base;
    auto* d3 = static_cast<const Derived*>(const_base);
    // CHECK-MESSAGES: :[[@LINE-1]]:16: warning: static_cast in Blink should be replaced with DynamicTo<T> [chromium-blink-cast]
    // CHECK-FIXES: auto* d3 = DynamicTo<Derived>(const_base);

    delete base;
    delete np;
}

} // namespace blink