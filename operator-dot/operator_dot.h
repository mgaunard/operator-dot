#ifndef OPERATOR_DOT_H_INCLUDED
#define OPERATOR_DOT_H_INCLUDED

#include <utility>
#include <cstddef>

template<class F1, class F2>
struct overloaded : F1, F2
{
	overloaded(F1 const& f1, F2 const& f2) : F1(f1), F2(f2) {}

	using F1::operator();
	using F2::operator();
};

template<class F1, class F2>
overloaded<F1, F2> overload(F1 const& f1, F2 const& f2)
{
	return overloaded<F1, F2>(f1, f2);
}

template<class T, class R = void>
struct sink { typedef R type; };

template<std::size_t I, class R = void>
struct sink2 { typedef R type; };

template<class Sig, class Enable = void>
struct is_callable : std::false_type {};

template<class F, class... Args>
struct is_callable<F(Args...), typename sink<decltype(std::declval<F>()(std::declval<Args>()...))>::type> : std::true_type {};

template<class F1, class F2>
struct overloaded_fallback
{
	overloaded_fallback(F1 const& f1_, F2 const& f2_) : f1(f1_), f2(f2_) {}

	F1 f1;
	F2 f2;

    template<class... Args>
    auto operator()(Args&&... args) -> decltype(f1(args...))
    {
    	return f1(std::forward<Args>(args)...);
    }

    template<class... Args>
    auto operator()(Args&&... args) -> typename std::enable_if<!is_callable<F1(Args&&...)>::value, decltype(f2(args...))>::type
    {
    	return f2(std::forward<Args>(args)...);
    }
};

template<class F1, class F2>
overloaded_fallback<F1, F2> overload_fallback(F1 const& f1, F2 const& f2)
{
	return overloaded_fallback<F1, F2>(f1, f2);
}

#define OPERATOR_DOT(obj, name)                                               \
(obj).operator_dot(                                                           \
	overload_fallback( [](auto&& _obj) -> decltype((_obj.name)) { return _obj.name; }  \
		             , [](auto&& _obj, typename sink2<sizeof(&std::remove_reference<decltype(_obj)>::type::name)>::type* = 0) { return [&_obj](auto&&... _args) -> decltype((_obj.name(std::forward<decltype(_args)>(_args)...))) { return _obj.name(std::forward<decltype(_args)>(_args)...); }; } \
                     )                                                                 \
)                                                                             \
/**/

#endif