#include "GL/GL.hpp"
#include "GLFW/GLFW.hpp"

#include "BL/scope.hpp"
#include "BL/timer.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "extern/stb_image.h"

#include "Renderer.hpp"
#include "InputHandler.hpp"
#include "Object.hpp"

#include "TestGame.hpp"

glm::mat4 getLockedCamera(glm::vec3 offset, const Object& obj) {
        auto view = glm::inverse(obj.objTransform);
        view = glm::rotate((float)M_PI, glm::vec3{0, 1, 0}) * view;
        view[3] += glm::vec4{offset, 0};
        return view;
}

int main() {
        glfwInit();
        scope_exit([&] { glfwTerminate(); });

        Renderer renderer;
        InputHandler input(renderer.glfwWindow);

        TestGame game;
        game.init();

        while(!renderer.glfwWindow.shouldClose()) {
                BL::Timer timer;

                input.update();
                if (input.getKey(GLFW_KEY_SPACE)) {
                        game = [] {
                                TestGame game;
                                game.init();
                                return game;
                        }();
                }
                game.update(input);
                game.render(renderer);

                renderer.glfwWindow.setTitle(std::string{"Hello, World!  FPS:"}.append(std::to_string(1000000.0f/(float)timer.elapsedUS())).append(")"));

                timer.roundTo(std::chrono::microseconds(16666));
        }
}
