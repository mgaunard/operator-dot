#include <operator_dot.h>
#include <utility>
#include <type_traits>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/thread/future.hpp>

using boost::future;
using boost::make_ready_future;
using boost::when_all;
using boost::async;

template<class T>
T&& unwrap(T&& t) { return static_cast<T&&>(t); }
template<class T>
T unwrap(future<T>&& t) { return t.get(); }
template<class T>
T unwrap(future<T>& t) { return t.get(); }
template<class T>
using unwrap_t = decltype(unwrap(std::declval<T>()));

template<class T>
future<typename std::remove_reference<T>::type> wrap(T&& t) { return make_ready_future<typename std::remove_reference<T>::type>(t); }
template<class T>
future<T>&& wrap(future<T>&& t) { return static_cast<future<T>&&>(t); }
template<class T>
future<T>& wrap(future<T>& t) { return t; }

template<class... T>
struct is_future
{
    static const bool value = false;
};

template<class T0, class... T>
struct is_future<T0, T...>
{
    static const bool value = !std::is_same<T0, unwrap_t<T0> >::value || is_future<T...>::value;
};

template<int ...>
struct seq { };

template<int N, int ...S>
struct gens : gens<N-1, N-1, S...> { };

template<int ...S>
struct gens<0, S...> {
  typedef seq<S...> type;
};

template<class F, class Tuple, int... S>
auto expand(F&& f, Tuple&& tuple, seq<S...>) -> decltype(f(std::get<S>(tuple).get()...))
{
    return f(std::get<S>(tuple).get()...);
}

template<class F, class Tuple>
auto expand(F&& f, Tuple&& tuple) -> decltype(expand(f, tuple, typename gens< std::tuple_size<Tuple>::value >::type()))
{
    return expand(f, tuple, typename gens< std::tuple_size<Tuple>::value >::type());
}

template<class T>
struct async_obj {
    template<class... Args>
    async_obj(Args&&... args) : obj(std::forward<Args>(args)...) {}

    template<class F, class... Args>
    auto operator_dot(F f, Args&&... args) -> future< decltype(f(std::declval<T&>(), unwrap(std::forward<Args>(args))...)) > {
        return is_future<Args...>::value ? 
               // move required here, might be a boost limitation
               when_all(wrap(obj), wrap(std::move(args))...).then([&](auto fut) { return expand(f, fut.get()); }) :
               async(std::bind(f, obj, unwrap(std::forward<Args>(args))...));
    }

private:
    T obj;
};

struct my_object
{
    double compute_approximation(double value) { return value; }
    double compute_solution(double value, double approximation) { return value; }
};

int main()
{
    async_obj<my_object> obj;
    future<double> approx = OPERATOR_DOT_FN(obj, compute_approximation, (42.));
    future<double> sol = OPERATOR_DOT_FN(obj, compute_solution, (42.)(approx));

    std::cout << sol.get() << std::endl;
}
