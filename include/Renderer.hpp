#pragma once

#include "GL/GL.hpp"
#include "GLFW/GLFW.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "AssetManager.hpp"
#include "Object.hpp"
#include "Scene.hpp"
#include "misc.hpp"

#include "BulletRenderer.hpp"
#include "ShadowmapRenderer.hpp"

class Renderer {
public:
        GLFW::Window glfwWindow{initWindow()};
        GL::Program renderProgram{initProgram()};
        GL::VertexArray vao{initVertexArray(renderProgram)};

        std::unique_ptr<ShadowmapRenderer> shadowRenderer{
                std::make_unique<ShadowmapRenderer>()
        };

        std::unique_ptr<BulletRenderer> bulletRenderer{
                std::make_unique<BulletRenderer>()
        };

        static GLFW::Window initWindow() {
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                GLFW::Window glfwWindow{1024, 768, "Hello, World!"};
                glfwWindow.makeCurrent();

                // ::glfwSwapInterval(0);

                glewExperimental = GL_TRUE;
                glewInit();
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);

                return glfwWindow;
        }

        Renderer() {
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
                renderProgram.uniform<int>("shadowmapArrayID", 4);

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

        void drawScene(const glm::mat4& camera, const Scene& scene) {
                glm::vec3 cameraPos { glm::inverse(camera) * glm::vec4(0,0,0,1) };

                shadowRenderer->drawShadowmaps(cameraPos, scene);

                renderProgram.use();
                renderProgram.uniform("camera", camera);
                renderProgram.uniform("cameraPos", cameraPos);
                renderProgram.uniform("lightDirection", scene.sunDirection);

                glActiveTexture(GL_TEXTURE4);
                auto& shadowmaps = shadowRenderer->shadowmaps;
                shadowmaps.shadowTex.bind();
                renderProgram.uniform("shadowTransform", shadowmaps.projView(0, cameraPos, scene.sunDirection));
                renderProgram.uniform<int>("numShadowLevels", shadowmaps.numShadowmaps());

                GL::Framebuffer::unbind();
                const glm::vec3& bg = scene.backgroundColor;
                glClearColor(bg.r, bg.g, bg.b, 1.0f);
                auto windowSize = glfwWindow.getSize();
                glViewport(0, 0, windowSize.x, windowSize.y);
                renderProgram.uniform("projection", glm::perspectiveFovRH(3.14159f/2.0f, windowSize.x, windowSize.y, 0.01f, 100.0f));
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                for (auto& obj : scene.objects) {
                        drawObject(*obj);
                }

                if (scene.bulletSystem.get() != nullptr) {
                        auto& bulletSystem = *scene.bulletSystem;
                        bulletSystem.prepareForDraw();
                        bulletRenderer->drawBullets(camera, bulletSystem.numBullets(), bulletSystem.getBuffer(), *bulletSystem.img);
                }

                glfwWindow.swapBuffers();
        }
};
