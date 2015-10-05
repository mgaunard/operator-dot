#include <operator_dot.h>

#include <iostream>

struct Foo
{
    template<class F, class... Args>
    void operator_dot(F&& f, Args&&... args) const
    {
        std::cout << F::name() << std::endl;
    }
};

int main()
{
    OPERATOR_DOT(Foo(), bar);
    OPERATOR_DOT_FN(Foo(), bar, );
    OPERATOR_DOT_FN(Foo(), bar, (1)(2));
}
