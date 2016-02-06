#include "operator_dot.h"
#include <stdexcept>
#include <iostream>

struct Thing {};

struct Baz
{
	double bar(int) { return 0.0; }
	//double bar(double) { return 1.0; }
	int baz;
};

struct Foo
{
	template<class F>
	auto operator_dot(F&& f)
	{
		return f(baz);
	}

	Baz baz;
};

int main()
{
	Foo foo;
	foo.baz.baz = 42;

	double value = OPERATOR_DOT(foo, bar)(1);
	int another_value = OPERATOR_DOT(foo, baz);

	std::cout << "value = " << value << std::endl;
	std::cout << "another_value = " << another_value << std::endl;
}