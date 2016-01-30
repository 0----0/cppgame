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

#include "Geometry.hpp"
#include "GeometryBuffer.hpp"
#include "Material.hpp"
#include "Object.hpp"
#include "Camera.hpp"

#include <vector>


std::string readFile(const char* filename) {
        std::string string;
        std::ifstream file(filename);
        file.seekg(0, std::ios::end);
        string.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&string[0], string.size());
        file.close();
        return string;
}

struct Scene {
        glm::vec3 backgroundColor;
        glm::vec3 sunDirection;
        std::vector<std::shared_ptr<Object>> objects;

        void update() {
                for(auto& obj : objects) {
                        obj->update();
                }
        }
};

struct Shadowmap {
        GL::Texture2D shadowTex;
        GL::Framebuffer shadowFbuff;
        uint xRes;
        uint yRes;
        float size;
        float depth;

        Shadowmap(uint xRes, uint yRes, float size, float depth):
                xRes(xRes), yRes(yRes), size(size), depth(depth)
        {

                shadowTex.assign(0, GL_DEPTH_COMPONENT16, xRes, yRes);
                float borderColor[4]{1.0f, 1.0f, 1.0f, 1.0f};
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

                shadowFbuff.attachTexture(GL_DEPTH_ATTACHMENT, shadowTex, 0);
        }

        glm::mat4 projView(glm::vec3 eye, glm::vec3 light) {
                auto view = glm::lookAt(eye, eye + light, {0,1,0});
                float off = size/2.0f;
                float doff = depth/2.0f;
                auto proj = glm::ortho(-off, off, -off, off, -doff, doff);
                return proj * view;
        }
};

struct Renderer {
        GLFW::Window glfwWindow{initWindow()};
        GL::Program renderProgram{initProgram()};
        GL::VertexArray vao{initVertexArray(renderProgram)};

        GL::Program shadowProg;
        GL::VertexArray shadowVao;
        // GL::Framebuffer shadowFbuff;
        // GL::Texture2D shadowTex;
        Shadowmap shadowmap{1024, 1024, 32.0f, 256.0f};
        Shadowmap shadowmap2{1024, 1024, 64.0f, 256.0f};

        static GLFW::Window initWindow() {
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
                GLFW::Window glfwWindow{1024, 768, "Hello, World!"};
                glfwWindow.makeCurrent();

                glewExperimental = GL_TRUE;
                glewInit();
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);

                return glfwWindow;
        }

        Renderer() {
                auto vert = GL::Shader::fromString(GL_VERTEX_SHADER, readFile("../shadows_vert.glsl"));
                shadowProg.attachShader(vert);
                shadowProg.link();
                shadowProg.use();

                shadowVao.vertexAttribFormat<glm::vec3>(shadowProg.attrib("vertPos"), 0);
                shadowVao.vertexAttribBinding(shadowProg.attrib("vertPos"), 0);
                shadowVao.enableVertexAttrib(shadowProg.attrib("vertPos"));

                // shadowTex.assign(0, GL_DEPTH_COMPONENT16, 1024, 1024);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
                //
                // shadowFbuff.attachTexture(GL_DEPTH_ATTACHMENT, shadowTex, 0);
        }

        static GL::Program initProgram() {
                GL::Program renderProgram;

                auto vert = GL::Shader::fromString(GL_VERTEX_SHADER, readFile("../vert.glsl"));
                auto frag = GL::Shader::fromString(GL_FRAGMENT_SHADER, readFile("../frag.glsl"));

                renderProgram.attachShader(vert);
                renderProgram.attachShader(frag);
                renderProgram.link();
                renderProgram.use();

                renderProgram.uniform("projection", glm::perspectiveFovRH(3.14159f/2.0f, 1024.0f, 768.0f, 0.01f, 100.0f));
                renderProgram.uniform("camera", glm::translate(glm::vec3(0, 0, -2)));
                renderProgram.uniform("obj", glm::mat4());

                renderProgram.uniform<int>("textureID", 0);
                renderProgram.uniform<int>("normMapID", 1);
                renderProgram.uniform<int>("shadowMapID", 4);

                return renderProgram;
        }

        static GL::VertexArray initVertexArray(GL::Program& renderProgram) {
                GL::VertexArray vao;

                auto setupVertexAttrib = [&](const char* name, bool normalize, int buff, auto _T) {
                        using T = decltype(_T);
                        auto attrib = renderProgram.attrib(name);
                        vao.vertexAttribFormat<T>(attrib, normalize);
                        vao.vertexAttribBinding(attrib, buff);
                        vao.enableVertexAttrib(attrib);
                };

                setupVertexAttrib("position", 0, 0, glm::vec3());
                setupVertexAttrib("vertNormal", 0, 1, glm::vec3());
                setupVertexAttrib("vertTex", 0, 2, glm::vec2());
                setupVertexAttrib("vertTangent", 0, 3, glm::vec3());
                setupVertexAttrib("vertBitangent", 0, 4, glm::vec3());

                return vao;
        }

        void drawObject(const Object& obj) {
                renderProgram.uniform("obj", obj.objTransform);
                obj.material->bind();
                obj.geometry->bind(vao);
                obj.geometry->draw();
        }

        void drawObjectShadow(const Object& obj) {
                shadowProg.uniform("obj", obj.objTransform);
                shadowVao.bindVertexBuffer<glm::vec3>(0, obj.geometry->vertices, 0);
                obj.geometry->draw();
        }

        void drawShadowmap(glm::vec3 cameraPos, const Scene& scene, Shadowmap& shMap) {
                glm::vec3 l = scene.sunDirection;
                // glm::vec3 upVector {0, 1, 0};
                // glm::mat4 projView = glm::lookAt(cameraPos,cameraPos + l,upVector);
                //
                // float off = shMap.size/2.0f;
                // projView = glm::ortho(-off, off, -off, off, -off, off) * projView;

                shadowProg.uniform("projView", shMap.projView(cameraPos, l));

                glViewport(0, 0, shMap.xRes, shMap.yRes);

                shMap.shadowFbuff.bind();
                glClear(GL_DEPTH_BUFFER_BIT);
                // glCullFace(GL_FRONT);
                for (auto& obj : scene.objects) {
                        drawObjectShadow(*obj);
                }
        }

        void drawScene(const glm::mat4& camera, const Scene& scene) {
                glm::vec3 cameraPos { glm::inverse(camera) * glm::vec4(0,0,0,1) };

                drawShadowmap(cameraPos, scene, shadowmap);
                drawShadowmap(cameraPos, scene, shadowmap2);

                renderProgram.use();
                renderProgram.uniform("camera", camera);
                renderProgram.uniform("cameraPos", cameraPos);
                renderProgram.uniform("lightDirection", scene.sunDirection);

                glActiveTexture(GL_TEXTURE4);
                shadowmap.shadowTex.bind();
                renderProgram.uniform("shadowTransform", shadowmap.projView(cameraPos, scene.sunDirection));

                GL::Framebuffer::unbind();
                const glm::vec3& bg = scene.backgroundColor;
                glClearColor(bg.r, bg.g, bg.b, 1.0f);
                glViewport(0, 0, 1024, 768);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // glCullFace(GL_BACK);

                for (auto& obj : scene.objects) {
                        drawObject(*obj);
                }

                glfwWindow.swapBuffers();
        }
};

glm::mat4 getLockedCamera(const Object& obj) {
        auto view = glm::inverse(obj.objTransform);
        view = glm::rotate((float)M_PI, glm::vec3{0, 1, 0}) * view;
        view[3] += glm::vec4{0, -1, -2, 0};
        return view;
}

static void focusCallback(GLFWwindow* window, int focused) {
        if (focused) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
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

int main() {
        glfwInit();
        scope_exit([&] { glfwTerminate(); });

        Renderer renderer;
        glfwSetInputMode(renderer.glfwWindow(), GLFW_STICKY_KEYS, GL_FALSE);
        glfwSetInputMode(renderer.glfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetWindowFocusCallback(renderer.glfwWindow(), focusCallback);
        // glfwSetKeyCallback(renderer.glfwWindow(), keyCallback);

        Geometry brick = Geometry::fromPly("../assets/legobrick.ply");
        auto buff = std::make_shared<GeometryBuffer>(brick);

        auto brickTexture = std::make_shared<GL::Texture2D>();
        {
                int w, h, n;
                unsigned char* data = stbi_load("../assets/textures/BrickTex.png", &w, &h, &n, 4);
                scope_exit([&] { stbi_image_free(data); });

                brickTexture->assign(0, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        auto brickNorms = std::make_shared<GL::Texture2D>();
        {
                int w, h, n;
                unsigned char* data = stbi_load("../assets/textures/BrickNormals.png", &w, &h, &n, 4);
                scope_exit([&] { stbi_image_free(data); });

                brickNorms->assign(0, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        auto brickMaterial = std::make_shared<Material>(
                brickTexture, brickNorms
        );

        auto obj = std::make_shared<Object>(glm::mat4(), buff, brickMaterial);
        auto obj2 = std::make_shared<Object>(glm::translate(glm::vec3(0, 2.0, 0)), buff, brickMaterial);

        Scene scene;
        scene.backgroundColor = glm::vec3(0.5f, 0.15f, 0.25f);
        scene.objects.push_back(obj);
        scene.objects.push_back(obj2);
        int monkeySize = 10;
        for (int i = 0; i < monkeySize; i++) { for (int j = 0; j < monkeySize; j++) {
                float x = (i - monkeySize/2) * 2;
                float z = (j + 1) * 2;
                auto obj = std::make_shared<Object>(glm::translate(glm::vec3(x, 0, z)), buff, brickMaterial);
                scene.objects.push_back(std::move(obj));
        }}

        float x = 0.0f;

        FiveDOFCamera camera;
        InputHandler input(renderer.glfwWindow);
        while(!renderer.glfwWindow.shouldClose()) {
                BL::Timer timer;

                input.update();
                scene.update();

                // auto camMatrix = glm::rotate((float)M_PI, glm::vec3{0, 1, 0});
                // obj->velocity += glm::vec3(obj->objTransform * camMatrix * glm::vec4(input.getMotionInput(), 0.0f)) * 0.005f;
                // obj->torque += input.get3DOFRotationInput() * 0.02f;

                scene.sunDirection = glm::vec3(glm::rotate(x,glm::vec3{0,1,0})*glm::vec4(glm::normalize(glm::vec3{-1,-1,-1}),1));
                x += 0.01f;

                camera.update(input);
                renderer.drawScene(camera.getMatrix(), scene);
                // renderer.drawScene(getLockedCamera(*obj), scene);

                renderer.glfwWindow.setTitle(std::string{"Hello, World!  FPS:"}.append(std::to_string(1000.0f/(float)timer.elapsedMS())).append(")"));

                timer.roundTo(std::chrono::milliseconds(16));
        }
}
