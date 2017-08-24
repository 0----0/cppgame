namespace nk {

namespace detail {

} // namespace detail

template<typename T>
struct wrapper_traits;

template<typename T>
class Wrapper {
protected:
        using traits = wrapper_traits<T>;
        using handle_type = typename traits::handle_type;
        handle_type handle;
public:
        Wrapper(handle_type handle): handle(handle) {}

        ~Wrapper() {
                traits::release(handle);
        }

        Wrapper(const Wrapper& rhs) = delete;
        Wrapper(Wrapper&& rhs) noexcept {
                handle = rhs.handle;
                rhs.handle = 0;
        }

        Wrapper& operator=(Wrapper&& rhs) {
                traits::release(handle);
                handle = rhs.handle;
                return *this;
        }

        handle_type& operator()() {
                return handle;
        }

        const handle_type& operator()() const {
                return handle;
        }
};

class context;

template<>
struct wrapper_traits<context> {
        using handle_type = struct nk_context*;

        static void release(handle_type handle) {

        }
};

class context {

};


} // namespace nk
