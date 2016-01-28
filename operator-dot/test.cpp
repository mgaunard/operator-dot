#include "operator_dot.h"
#include <stdexcept>
#include <iostream>

struct Thing {};

struct Baz
{
	double bar(int) { return 0.0; }
	//double bar(double) { return 1.0; }
	int baz;

	Thing thing;
};

struct any
{
	template<class... Args>
	any operator()(Args&&...)
	{
		__builtin_trap();
		return any();
	}

	template<class T>
	operator T()
	{
		__builtin_trap();
		return *(T*)0;
	}
};

struct throw_
{
	throw_(std::string const& msg_) : msg(msg_) {}
	std::string msg;

    template<class Args>
    any operator()(Args&&...) const
    {
        throw std::runtime_error(msg);
    }
};

struct Foo
{
	template<class F, class T, class Enable = void>
	struct operator_dot_impl
	{
		static auto call(F f, T t)
		{
			return overload_fallback(f, throw_("No such member"))(t);
		}
	};

	template<class F, class T>
	struct operator_dot_impl<F, T, typename std::enable_if< std::is_class< decltype(std::declval<F>()(std::declval<T>())) >::value >::type>
	{
		static auto call(F f, T t)
		{
			return overload_fallback(f(t), throw_("no such signature"));
		}
	};

	template<class F>
	auto operator_dot(F&& f)
	{
		return operator_dot_impl<F&&, Baz&>::call(std::forward<F>(f), baz);
	}

	Baz baz;
};

int main()
{
	Foo foo;
	double value = OPERATOR_DOT(foo, bar)(1);
	double value2 = OPERATOR_DOT(foo, bar)(1, 2);
	double value3 = OPERATOR_DOT(foo, lol)(1, 2, 3);
	int another_value = OPERATOR_DOT(foo, baz);
	Thing some_thing = OPERATOR_DOT(foo, thing);
}