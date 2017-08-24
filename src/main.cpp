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

#include "TestGame2Wrp.hpp"

#include "LuaInterface.hpp"

#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear/nuklear.h"

#include "GUI/GUIManager.hpp"
#include "GUI/ColorPicker.hpp"

class FPSDisplayWindow : public INKWindow {

        virtual void override draw() {
                if (nk_begin(ctx, "Scene Preferences", nk_rect(50, 50, 230, 400),
                                    NK_WINDOW_BORDER|NK_WINDOW_MOVABLE
                                    |NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE
                                    |NK_WINDOW_TITLE))
                {

                }
        }
};

int main() {
        glfwInit();
        scope_exit([&] { glfwTerminate(); });

        Renderer renderer;
        LuaInterface lua;

        auto keymap = std::make_shared<Keymap>(Keymap({
                {"FORWARD", GLFW_KEY_UP},
                {"LEFT", GLFW_KEY_LEFT},
                {"BACK", GLFW_KEY_DOWN},
                {"RIGHT", GLFW_KEY_RIGHT},
                {"FIRE", GLFW_KEY_Z}
        }));
        InputHandler input(renderer.glfwWindow, keymap);

        struct nk_context* ctx = nk_glfw3_init(renderer.glfwWindow(), NK_GLFW3_INSTALL_CALLBACKS);
        scope_exit([&] { nk_glfw3_shutdown(); });

        {
                struct nk_font_atlas *atlas;
                nk_glfw3_font_stash_begin(&atlas);
                struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "./DroidSans.ttf", 14, 0);
                nk_glfw3_font_stash_end();
                nk_style_set_font(ctx, &droid->handle);
        }


        TestGame2Wrp game;
        game.init();
        while(!renderer.glfwWindow.shouldClose()) {
                BL::Timer timer;

                input.update();
                if (input.getKey(GLFW_KEY_SPACE)) {
                        // game = [] {
                        //         TestGame2Wrp game;
                        //         game.init();
                        //         return game;
                        // }();
                        game.init();
                }
                if (!input.getKey(GLFW_KEY_B))
                        game.update(input);
                game.render(renderer);

                nk_glfw3_new_frame();
                GUIManager::get().draw(ctx);
                nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);

                renderer.glfwWindow.swapBuffers();

                // renderer.glfwWindow.setTitle(std::string{"Hello, World!  FPS:"}.append(std::to_string(1000000.0f/(float)timer.elapsedUS())).append(")"));

                // timer.roundTo(std::chrono::microseconds(16666));
                // timer.roundTo(std::chrono::nanoseconds(16666666));
        }
}
