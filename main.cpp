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
#include "TestGame2Wrp.hpp"

// class GUIObject {
//
// };
//
// class GameWrapper {
//         static std::shared_ptr<Keymap> keymap;
//
//         TestGame2Wrp game;
//         InputHandler input;
//
//         GameWrapper(Renderer& renderer):
//                 input(renderer.glfwWindow, keymap)
//         {
//
//         }
// };

int main() {
        glfwInit();
        scope_exit([&] { glfwTerminate(); });

        Renderer renderer;

        auto keymap = std::make_shared<Keymap>(Keymap({
                {"FORWARD", GLFW_KEY_UP},
                {"LEFT", GLFW_KEY_LEFT},
                {"BACK", GLFW_KEY_DOWN},
                {"RIGHT", GLFW_KEY_RIGHT},
                {"FIRE", GLFW_KEY_Z}
        }));
        InputHandler input(renderer.glfwWindow, keymap);

        TestGame2Wrp game;
        game.init();

        while(!renderer.glfwWindow.shouldClose()) {
                BL::Timer timer;

                input.update();
                if (input.getKey(GLFW_KEY_SPACE)) {
                        game = [] {
                                TestGame2Wrp game;
                                game.init();
                                return game;
                        }();
                }
                if (!input.getKey(GLFW_KEY_B))
                        game.update(input);
                game.render(renderer);
                
                renderer.glfwWindow.setTitle(std::string{"Hello, World!  FPS:"}.append(std::to_string(1000000.0f/(float)timer.elapsedUS())).append(")"));

                timer.roundTo(std::chrono::microseconds(16666));
        }
}
