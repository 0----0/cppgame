#ifndef _GLFW_HPP
#define _GLFW_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <functional>

namespace GLFW {

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

class Window;

template<>
struct wrapper_traits<Window> {
        using handle_type = GLFWwindow*;

        static void release(handle_type handle) {
                glfwDestroyWindow(handle);
        }
};

namespace detail {

template<typename F>
struct closurize_callback_ptr;

template<typename R, typename... ArgsT>
struct closurize_callback_ptr<R(*)(GLFWwindow*, ArgsT...)> {
        using type = std::function<R(Window&, ArgsT...)>;
};

template<typename F>
using closurize_callback_ptr_t = typename closurize_callback_ptr<F>::type;


template<typename F, closurize_callback_ptr_t<F> Window::* M>
struct call_closure;

template<typename R, typename... ArgsT, closurize_callback_ptr_t<R(*)(GLFWwindow*, ArgsT...)> Window::* M>
struct call_closure<R(*)(GLFWwindow*, ArgsT...), M> {
        static R func(GLFWwindow* window, ArgsT... args) {
                auto obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
                (obj->*M)(*obj, args...);
        }
};

};

class Window: public Wrapper<Window> {
private:
        detail::closurize_callback_ptr_t<GLFWwindowsizefun> sizelambda;
        detail::closurize_callback_ptr_t<GLFWwindowfocusfun> focuslambda;
        detail::closurize_callback_ptr_t<GLFWmousebuttonfun> mouselambda;
        detail::closurize_callback_ptr_t<GLFWkeyfun> keylambda;
public:
        Window(int width, int height, const char* title):
                Wrapper<Window>(::glfwCreateWindow(width, height, title, NULL, NULL))
        {
                ::glfwSetWindowUserPointer(handle, this);
        }

        Window(Window&& rhs): Wrapper(std::move(rhs)) {
                ::glfwSetWindowUserPointer(handle, this);
        }

        Window& operator=(Window&& rhs) {
                *(Wrapper*)this = std::move(rhs);
                ::glfwSetWindowUserPointer(handle, this);
                return *this;
        }

        void setTitle(const char* title) {
                ::glfwSetWindowTitle(handle, title);
        }

        void setTitle(const std::string& title) {
                setTitle(title.c_str());
        }

        glm::vec2 getSize() {
                glm::ivec2 size;
                glfwGetWindowSize(handle, &size.x, &size.y);
                return size;
        }

        void makeCurrent() const {
                ::glfwMakeContextCurrent(handle);
        }

        bool shouldClose() const {
                return ::glfwWindowShouldClose(handle);
        }

        void swapBuffers() const {
                ::glfwSwapBuffers(handle);
        }

        bool getKey(int key) const {
                return ::glfwGetKey(handle, key) == GLFW_PRESS;
        }

        bool getMouseButton(int key) const {
                return ::glfwGetMouseButton(handle, key) == GLFW_PRESS;
        }

        glm::vec2 getCursorPos() const {
                double xpos, ypos;
                ::glfwGetCursorPos(handle, &xpos, &ypos);
                return {xpos, ypos};
        }

        void setSizeCallback(decltype(sizelambda) f) {
                sizelambda = std::move(f);
                ::glfwSetWindowSizeCallback(handle, detail::call_closure<GLFWwindowsizefun, &Window::sizelambda>::func);
        }
        void setFocusCallback(decltype(focuslambda) f) {
                focuslambda = std::move(f);
                ::glfwSetWindowFocusCallback(handle, detail::call_closure<GLFWwindowfocusfun, &Window::focuslambda>::func);
        }
        void setMouseButtonCallback(decltype(mouselambda) f) {
                mouselambda = std::move(f);
                ::glfwSetMouseButtonCallback(handle, detail::call_closure<GLFWmousebuttonfun, &Window::mouselambda>::func);
        }
        void setKeyCallback(decltype(keylambda) f) {
                keylambda = std::move(f);
                ::glfwSetKeyCallback(handle, detail::call_closure<GLFWkeyfun, &Window::keylambda>::func);
        }
};

}; //namespace GLFW

#endif //_GLFW_HPP
