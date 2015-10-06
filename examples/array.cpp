#include <operator_dot.h>
#include <utility>
#include <vector>
#include <cstddef>
#include <complex>

template<class T>
struct array {
    template<class F, class... Args>
    auto operator_dot(F f, Args&&... args) -> array< decltype(f(std::declval<T&>(), std::forward<Args>(args)...)) > {
        array< decltype(f(values[0], std::forward<Args>(args)...)) > result;
        result.values.resize(values.size());
        for(size_t i=0; i<values.size(); ++i) {
            result.values[i] = f(values[i]);
        }
        return result;
    }

private:
    std::vector<T> values;

    template<class U>
    friend struct array;
};

int main()
{
    array< std::complex<float> > complex_numbers;
    array< float > real_part = OPERATOR_DOT_FN(complex_numbers, real, );
    array< float > imaginary_part = OPERATOR_DOT_FN(complex_numbers, imag, );
}
