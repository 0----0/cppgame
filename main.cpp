#include "GL/GL.hpp"
#include "GLFW/GLFW.hpp"

#include "BL/scope.hpp"
#include "BL/timer.hpp"
#include "BL/stream.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/projection.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "extern/stb_image.h"

#include "AssetManager.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Geometry.hpp"
#include "GeometryBuffer.hpp"
#include "Material.hpp"
#include "Object.hpp"
#include "Camera.hpp"

#include <vector>
#include <initializer_list>

glm::mat4 getLockedCamera(glm::vec3 offset, const Object& obj) {
        auto view = glm::inverse(obj.objTransform);
        view = glm::rotate((float)M_PI, glm::vec3{0, 1, 0}) * view;
        view[3] += glm::vec4{offset, 0};
        return view;
}

template<typename T>
inline std::shared_ptr<T> share(T&& obj) {
        return std::make_shared<T>(std::forward<T>(obj));
}

class TestGame {
        std::unique_ptr<Scene> scene;
        std::shared_ptr<const GeometryBuffer> brickGeo;
        std::shared_ptr<const GeometryBuffer> shipGeo;

        std::shared_ptr<Material> brickMat;
        std::shared_ptr<Material> shipMat;

        std::shared_ptr<Object> ship;

        FiveDOFCamera camera;
        float sunRotation;
public:
        void init() {
                scene = std::make_unique<Scene>();
                scene->backgroundColor = glm::vec3(0.5f, 0.15f, 0.25f);

                brickGeo = AssetManager::get().getModelBuffer("legobrick.ply");
                shipGeo  = AssetManager::get().getModelBuffer("ship1 v3.ply");

                auto brickDiffuse = AssetManager::get().getImage("BrickTex.png");
                auto brickNormals = AssetManager::get().getImage("BrickNormals2.png");
                brickMat = std::make_shared<Material>(brickDiffuse, brickNormals);
                shipMat = brickMat;

                auto gridSize = 10;
                auto gridSpacing = 2.f;
                for (int i = 0; i < gridSize; i++) { for (int j = 0; j < gridSize; j++) {
                        float x = (i - gridSize/2) * gridSpacing;
                        float z = (j + 1) * gridSpacing;
                        auto obj = std::make_shared<Object>(glm::translate(glm::vec3(x, 0, z)), brickGeo, brickMat);
                        scene->objects.push_back(std::move(obj));
                }}

                ship = std::make_unique<Object>(glm::mat4(), shipGeo, shipMat);
                scene->objects.push_back(ship);

                sunRotation = 0.0f;
                camera = FiveDOFCamera();

                ship->objTransform = glm::translate(glm::vec3{0, 10, 10});;
        }

        void update(InputHandler& input) {
                scene->update();

                scene->sunDirection = glm::vec3(glm::rotate(sunRotation,glm::vec3{0,1,0})*glm::vec4(glm::normalize(glm::vec3{-1,-1,-1}),1));
                if (input.getKey(GLFW_KEY_F)) {
                        sunRotation += 0.01f;
                }

                camera.update(input);
        }

        void render(Renderer& renderer) {
                renderer.drawScene(camera.getMatrix(), *scene);
        }
};

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
