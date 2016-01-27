#include <istream>

namespace BL {

template<typename T>
T stream_read(std::istream& stream) {
        T temp;
        stream >> temp;
        return temp;
}

namespace detail {

template<typename T>
struct arg_type;

template<typename R, typename T>
struct arg_type<R(T)> {
        using type = T;
};

template<typename T>
using arg_type_t = typename arg_type<T>::type;


template<typename T>
struct method_arg_type;

template<typename L, typename R, typename T>
struct method_arg_type<R(L::*)(T)> {
        using type = T;
};

template<typename L, typename R, typename T>
struct method_arg_type<R(L::*)(T) const> : public method_arg_type<R(L::*)(T)> {
};

template<typename L>
using lambda_arg_type_t = typename method_arg_type<decltype(&L::operator())>::type;

template<typename F>
class sink {
private:
        F func;
public:
        sink(F func): func(std::move(func)) {}
        friend std::istream& operator>>(std::istream& stream, const sink& rhs) {
                using data_type = lambda_arg_type_t<F>;
                data_type temp = stream_read<data_type>(stream);
                rhs.func(temp);
                return stream;
        }
};

template<typename T>
class back_sink {
private:
        T& ref;
public:
        constexpr back_sink(T& ref): ref(ref) {}
        friend std::istream& operator>>(std::istream& stream, const back_sink& sink) {
                using data_type = std::remove_reference_t<decltype(*(sink.ref.begin()))>;
                data_type temp;
                stream >> temp;
                sink.ref.push_back(std::move(temp));
                return stream;
        }
};

}; // namespace detail

// template<typename T>
// auto back_sink(T& ref) {
//         return detail::back_sink<T>(ref);
// }

template<typename F>
auto sink(F func) {
        return detail::sink<F>(std::move(func));
}

template<typename T>
auto back_sink(T& ref) {
        return sink([&ref](uint arg) { ref.push_back(arg); });
}

}; // namespace BL
