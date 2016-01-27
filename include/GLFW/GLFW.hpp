#ifndef _GLFW_HPP
#define _GLFW_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

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

class Window: public Wrapper<Window> {
public:
        Window(int width, int height, const char* title):
                Wrapper<Window>(::glfwCreateWindow(width, height, title, NULL, NULL))
        {
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
};

}; //namespace GLFW

#endif //_GLFW_HPP
