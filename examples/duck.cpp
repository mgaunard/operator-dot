#include <operator_dot.h>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <iostream>

template<class T, class R = void>
struct sink { typedef R type; };

template<class Sig, class Enable = void>
struct is_callable : std::false_type {};

template<class F, class... Args>
struct is_callable<F(Args...), typename sink<decltype(std::declval<F>()(std::declval<Args>()...))>::type> : std::true_type {};

template<class F, class R>
struct call_or_throw : F {

    call_or_throw(F f) : F(f) {}

    using F::operator();

    template<class T, class... Args>
    typename std::enable_if<!is_callable<F(T&&, Args&&...)>::value, R>::type operator()(T&& t, Args&&...) const
    {
        throw std::runtime_error("No such operation");
    }
};

template<class Sig, class... Types>
struct collapse_result_of;

template<class F, class... Args, class Type>
struct collapse_result_of<F(Args...), Type>
     : std::result_of<F(Type, Args...)>
{
};

template<class F, class... Args, class Type, class... Types>
struct collapse_result_of<F(Args...), Type, Types...>
     : collapse_result_of<F(Args...), Types...>
     , std::result_of<F(Type, Args...)>
{};

template<int N, class U, bool IsSame, class... T>
struct find_offset_impl;

template<int N, class U, class... T>
struct find_offset_impl<N, U, true, T...>
{
    static const int value = N;
};

template<int N, class U, class T0, class... T>
struct find_offset_impl<N, U, false, T0, T...>
     : find_offset_impl<N+1, U, std::is_same<T0, U>::value, T...>
{
};

template<class U, class... T>
struct find_offset;

template<class U, class T0, class... T>
struct find_offset<U, T0, T...> : find_offset_impl<0, U, std::is_same<U, T0>::value, T...> {};

template<int N, class... T>
struct get;

template<class T0, class... T>
struct get<0, T0, T...> {
    typedef T0 type;
};

template<int N, class T0, class... T>
struct get<N, T0, T...> {
    typedef typename get<N-1, T...>::type type;
};

// past the end, give a valid type to still build
template<int N, class T0>
struct get<N, T0> {
    typedef T0 type;
};
template<class T0>
struct get<0, T0> {
    typedef T0 type;
};

struct delete_visitor
{
    typedef void result_type;

    template<class T>
    void operator()(T& t) const
    {
        delete &t;
    }
};

struct clone_visitor
{
    template<class T>
    void* operator()(T const& t) const
    {
        return new T(t);
    }
};

template<class... T>
struct duck_variant {

private:
    void* ptr;
    int which;

public:
    template<class U>
    duck_variant(U const& u) : ptr(new U(u)), which(find_offset<U, T...>::value) {}

    duck_variant& operator=(duck_variant other) {
        using std::swap;
        swap(ptr, other.ptr);
        swap(which, other.which);
        return *this;
    }
    
    // collapse_result_of ensures the overload is only valid if all cases return the same type
    template<class F, class... Args>
    auto operator_dot(F f, Args&&... args) -> typename collapse_result_of<F(Args&&...), T&...>::type {

        typedef typename collapse_result_of<F(Args&&...), T&...>::type R;

        switch(which)
        {
            case 0: return call_or_throw<F, R>{f}(*static_cast<typename get<0, T...>::type*>(ptr), std::forward<Args>(args)...);
            case 1: return call_or_throw<F, R>{f}(*static_cast<typename get<1, T...>::type*>(ptr), std::forward<Args>(args)...);
            case 2: return call_or_throw<F, R>{f}(*static_cast<typename get<2, T...>::type*>(ptr), std::forward<Args>(args)...);
            case 3: return call_or_throw<F, R>{f}(*static_cast<typename get<3, T...>::type*>(ptr), std::forward<Args>(args)...);
            /* for every Ti... */
        }
    }

    template<class F, class... Args>
    auto operator_dot(F f, Args&&... args) const -> typename collapse_result_of<F(Args&&...), T const&...>::type {

        typedef typename collapse_result_of<F(Args&&...), T const&...>::type R;

        switch(which)
        {
            case 0: return call_or_throw<F, R>{f}(*static_cast<typename get<0, T...>::type const*>(ptr), std::forward<Args>(args)...);
            case 1: return call_or_throw<F, R>{f}(*static_cast<typename get<1, T...>::type const*>(ptr), std::forward<Args>(args)...);
            case 2: return call_or_throw<F, R>{f}(*static_cast<typename get<2, T...>::type const*>(ptr), std::forward<Args>(args)...);
            case 3: return call_or_throw<F, R>{f}(*static_cast<typename get<3, T...>::type const*>(ptr), std::forward<Args>(args)...);
            /* for every Ti... */
        }
    }

    ~duck_variant() { operator_dot(delete_visitor()); }

    duck_variant(duck_variant const& other) : ptr(other.operator_dot(clone_visitor())), which(other.which) {}
};

struct Foo { const char* name() { return "Foo"; } };
struct Bar { const char* name() { return "Bar"; } void bark() { std::cout << name() << std::endl; } };

int main()
{   
    duck_variant<Foo, Bar> v = Foo();
    v = Bar();
    const char* s = OPERATOR_DOT_FN(v, name, );
    std::cout << s << std::endl;
    OPERATOR_DOT_FN(v, bark, ); // error: not all types define 'bark'
}
