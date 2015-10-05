#include <operator_dot.h>
#include <utility>
#include <iostream>

template<class T>
struct poly {
    template<class D>
    poly(D const& t) : ptr(new D(t)) {}
    ~poly() { delete ptr; }

    poly(const poly& other) : ptr(other.ptr->clone()) {}

    poly& operator=(poly other) {
        std::swap(ptr, other.ptr);
        return *this;
    }

    template<class F, class... Args>
    auto operator_dot(F f, Args&&... args) -> decltype(f(std::declval<T&>(), std::forward<Args>(args)...)) {
        return f(*ptr, std::forward<Args>(args)...);
    }

    template<class F, class... Args>
    auto operator_dot(F f, Args&&... args) const -> decltype(f(std::declval<T const&>(), std::forward<Args>(args)...)) {
        return f(static_cast<T const&>(*ptr), std::forward<Args>(args)...);
    }

private:
    T* ptr;
};

struct Base
{
    virtual Base* clone() const { return new Base; }

    virtual void virtual_member_function_of_Base()
    {
        std::cout << "Base" << std::endl;
    }

    int member_variable_of_Base;
};

struct Derived1 : Base
{
    Derived1* clone() const { return new Derived1; }

    void virtual_member_function_of_Base()
    {
        std::cout << "Derived1" << std::endl;
    }
};

struct Derived2 : Base
{
    Derived2* clone() const { return new Derived2; }

    void virtual_member_function_of_Base()
    {
        std::cout << "Derived2" << std::endl;
    }
};

int main()
{
    poly<Base> a = Derived1();
    OPERATOR_DOT_FN(a, virtual_member_function_of_Base, );
    a = Derived2();
    int x = OPERATOR_DOT(a, member_variable_of_Base);
}
