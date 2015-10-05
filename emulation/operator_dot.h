#ifndef OPERATOR_DOT_H_INCLUDED
#define OPERATOR_DOT_H_INCLUDED

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

#define BOOST_PP_SEQ_ENUM_0

template<class F>
struct operator_dot_impl : F
{
	operator_dot_impl(const char* name_, F const& f) : F(f)
	{
		d_name = name_;
	}

	using F::operator();

	static const char* name()
	{
		return d_name;
	}

private:
	static const char* d_name;
};

template<class F>
const char* operator_dot_impl<F>::d_name = 0;

template<class F>
operator_dot_impl<F> make_operator_dot_impl(const char* name, F const& f)
{
	return operator_dot_impl<F>(name, f);
}

#define OPERATOR_DOT(obj, name) obj.operator_dot(make_operator_dot_impl(#name, [&](auto&& __obj) -> decltype(__obj.name) { return __obj.name; }))
#define OPERATOR_DOT_FN(obj, name, args) obj.operator_dot(make_operator_dot_impl(#name, [&](auto&& __obj, auto&&... __args) -> decltype(__obj.name(__args...)) { return __obj.name(__args...); }) BOOST_PP_COMMA_IF(BOOST_PP_SEQ_SIZE(args)) BOOST_PP_SEQ_ENUM(args))

#endif
