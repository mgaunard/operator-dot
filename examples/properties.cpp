#include <operator_dot.h>
#include <unordered_map>
#include <string>
#include <vector>

#include <boost/variant.hpp>

using boost::variant;
using boost::get;

struct Value {
    Value() : data(Map()) {}
    Value(int i) : data(i) {}
    Value(double d) : data(d) {}
    Value(std::string const& s) : data(s) {}
    Value(const char* s) : data(std::string(s)) {}

    template<class F>
    Value& operator_dot(F f) {
         std::vector<Value>& v = get<Map>(data)[F::name()];
         v.resize(1);
         return v[0];
    }

    template<class F>
    Value const& operator_dot(F f) const {
         return get<Map>(data)[F::name()].at(0);
    }

    friend std::ostream& operator<<(std::ostream& os, Value const& value)
    {
        return boost::apply_visitor(value_display(os), value.data);
    }

private:
    typedef std::unordered_map<std::string, std::vector<Value> > Map; // use vector to work around completeness requirements
    variant<Map, int, double, std::string> data;

    // pretty-printing
    struct value_display
    {
        value_display(std::ostream& os_) : os(os_) {}

        typedef std::ostream& result_type;

        template<class T>
        result_type operator()(T const& t) const
        {
            return os << t;
        }

        result_type operator()(std::string const& s) const
        {
            return os << "\"" << s << "\"";
        }

        result_type operator()(Map const& map) const
        {
            bool first = true;

            os << "{";
            for(auto it = map.begin(); it != map.end(); ++it)
            {
                if(!first)
                    os << ", ";
                else
                    first = false;

               (*this)(it->first);
               os << ": ";
               (*this)(it->second[0]);
            }
            return os << "}";
        }

    private:
        std::ostream& os;
    };
};

int main()
{
    Value v;
    OPERATOR_DOT(v, foo) = 42;
    OPERATOR_DOT(v, bar) = "bar";

    std::cout << v << std::endl;
}
