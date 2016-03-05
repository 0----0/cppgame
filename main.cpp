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

static void keyCallback(GLFWwindow* window, int key, int scnacode, int action, int mods) {
        if(key == GLFW_KEY_W) {
                std::cout << "W ";
                switch(action) {
                case GLFW_PRESS:
                        std::cout << "Press" << std::endl;
                        break;
                case GLFW_RELEASE:
                        std::cout << "Release" << std::endl;
                        break;
                case GLFW_REPEAT:
                        std::cout << "Repeat" << std::endl;
                        break;
                default:
                        std::cout << "???" << std::endl;
                }
        }
        else if(key == GLFW_KEY_E) {
                std::cout << "E ";
                switch(action) {
                case GLFW_PRESS:
                        std::cout << "Press" << std::endl;
                        break;
                case GLFW_RELEASE:
                        std::cout << "Release" << std::endl;
                        break;
                case GLFW_REPEAT:
                        std::cout << "Repeat" << std::endl;
                        break;
                default:
                        std::cout << "???" << std::endl;
                }
        }
}

static GL::Texture2D textureFromImage(const std::string& filename) {
        int w, h, n;
        unsigned char* data = stbi_load(filename.c_str(), &w, &h, &n, 0);
        if (!data) { throw std::runtime_error(std::string{"Failed to load image: "}.append(filename)); }
        scope_exit([&] { stbi_image_free(data); });
        if (!w || !h) { throw std::runtime_error(std::string{"Empty image: "}.append(filename)); }

        GLenum type;
        switch (n) {
        case 1:
                type = GL_RED;
                break;
        case 2:
                type = GL_RG;
                break;
        case 3:
                type = GL_RGB;
                break;
        case 4:
                type = GL_RGBA;
                break;
        default:
                throw std::runtime_error(std::string{filename}.append(": Unknown number of components: ").append(std::to_string(n)));
        }

        GL::Texture2D tex;
        tex.assign(0, type, w, h, type, GL_UNSIGNED_BYTE, data);
        tex.generateMipmap();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return tex;
}

template<typename T>
inline std::shared_ptr<T> share(T&& obj) {
        return std::make_shared<T>(std::forward<T>(obj));
}

class TestGame {
        std::unique_ptr<Scene> scene;
        std::shared_ptr<GeometryBuffer> brickGeo;
        std::shared_ptr<GeometryBuffer> shipGeo;

        std::shared_ptr<Material> brickMat;
        std::shared_ptr<Material> shipMat;

        std::shared_ptr<Object> ship;

        FiveDOFCamera camera;
        float sunRotation;
public:
        void init() {
                scene = std::make_unique<Scene>();
                scene->backgroundColor = glm::vec3(0.5f, 0.15f, 0.25f);

                brickGeo = std::make_shared<GeometryBuffer>(Geometry::fromPly("../assets/legobrick.ply"));
                shipGeo  = std::make_shared<GeometryBuffer>(Geometry::fromPly("../assets/ship1 v3.ply"));

                auto brickDiffuse = std::make_shared<GL::Texture2D>(textureFromImage("../assets/textures/BrickTex.png"));
                auto brickNormals = std::make_shared<GL::Texture2D>(textureFromImage("../assets/textures/BrickNormals2.png"));
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
                        game = TestGame();
                        game.init();
                }
                game.update(input);
                game.render(renderer);

                renderer.glfwWindow.setTitle(std::string{"Hello, World!  FPS:"}.append(std::to_string(1000000.0f/(float)timer.elapsedUS())).append(")"));

                timer.roundTo(std::chrono::microseconds(16666));
        }
}
