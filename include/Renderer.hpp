#pragma once

#include "GL/GL.hpp"
#include "GLFW/GLFW.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/projection.hpp>

#include "AssetManager.hpp"
#include "Object.hpp"
#include "Scene.hpp"
#include "misc.hpp"

struct ShadowmapArray {
        GL::Texture2DArray shadowTex;
        std::vector<GL::Framebuffer> shadowFbuffs;
        std::vector<float> sizes;
        uint xRes;
        uint yRes;
        float depth;

        ShadowmapArray(uint xRes, uint yRes, std::initializer_list<float> sizes, float depth):
                sizes(sizes),
                xRes(xRes),
                yRes(yRes),
                depth(depth)
        {
                shadowTex.assign(0, GL_DEPTH_COMPONENT16, xRes, yRes, sizes.size());

                float borderColor[4]{1.0f, 1.0f, 1.0f, 1.0f};
                glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                // glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                shadowFbuffs.reserve(sizes.size());
                for(int i = 0; i < sizes.size(); i++) {
                        shadowFbuffs.emplace_back();
                        shadowFbuffs.back().attachTextureLayer(GL_DEPTH_ATTACHMENT, shadowTex, 0, i);
                }
        }

        glm::mat4 projView(uint i, glm::vec3 eye, glm::vec3 light) const {
                auto view = glm::lookAt(eye, eye + light, {0,1,0});
                float off = sizes[i]/2.0f;
                float doff = depth/2.0f;
                auto proj = glm::ortho(-off, off, -off, off, -doff, doff);
                return proj * view;
        }

        uint numShadowmaps() const {
                return shadowFbuffs.size();
        }
};

class ShadowmapRenderer {
public:
        GL::Program shadowProg;
        GL::VertexArray shadowVao;
        ShadowmapArray shadowmaps {1024, 1024, {8, 16, 32, 64, 128, 256}, 256};

        ShadowmapRenderer() {
                auto vert = GL::Shader::fromString(GL_VERTEX_SHADER, readFile("../shadows_vert.glsl"));
                shadowProg.attachShader(vert);
                shadowProg.link();
                shadowProg.use();

                shadowVao.vertexAttribFormat<glm::vec3>(shadowProg.attrib("vertPos"), 0);
                shadowVao.vertexAttribBinding(shadowProg.attrib("vertPos"), 0);
                shadowVao.enableVertexAttrib(shadowProg.attrib("vertPos"));
        }

        void drawObjectShadow(const Object& obj) {
                shadowProg.uniform("obj", obj.objTransform);
                shadowVao.bindVertexBuffer<glm::vec3>(0, obj.geometry->vertices, 0);
                obj.geometry->draw();
        }

        void drawShadowmaps(glm::vec3 cameraPos, const Scene& scene) {
                glm::vec3 l = scene.sunDirection;

                glViewport(0, 0, shadowmaps.xRes, shadowmaps.yRes);
                for (int i = 0; i < shadowmaps.numShadowmaps(); ++i) {
                        shadowProg.uniform("projView", shadowmaps.projView(i, cameraPos, l));
                        shadowmaps.shadowFbuffs[i].bind();
                        glClear(GL_DEPTH_BUFFER_BIT);
                        for(auto& obj : scene.objects) {
                                drawObjectShadow(*obj);
                        }
                }
        }
};

class BulletRenderer {
public:
        GL::Program bulletProg;
        GL::VertexArray bulletVao;
        GL::TextureBuffer bulletPosTex;

        std::shared_ptr<const GeometryBuffer> quad {
                AssetManager::get().getModelBuffer("quad.ply")
        };

        BulletRenderer() {
                auto vert = GL::Shader::fromString(GL_VERTEX_SHADER, readFile("../bullet_vert.glsl"));
                auto frag = GL::Shader::fromString(GL_FRAGMENT_SHADER, readFile("../bullet_frag.glsl"));
                bulletProg.attachShader(vert);
                bulletProg.attachShader(frag);
                bulletProg.link();
                bulletProg.use();

                bulletProg.uniform<int>("textureID", 0);
                bulletProg.uniform<int>("bulletList", 5);

                bulletProg.uniform("projection", glm::perspectiveFovRH(3.14159f/2.0f, 1024.0f, 768.0f, 0.01f, 100.0f));

                bulletVao.vertexAttribFormat<glm::vec3>(bulletProg.attrib("vertPos"), 0);
                bulletVao.vertexAttribBinding(bulletProg.attrib("vertPos"), 0);
                bulletVao.enableVertexAttrib(bulletProg.attrib("vertPos"));

                bulletVao.vertexAttribFormat<glm::vec2>(bulletProg.attrib("vertTex"), 0);
                bulletVao.vertexAttribBinding(bulletProg.attrib("vertTex"), 2);
                bulletVao.enableVertexAttrib(bulletProg.attrib("vertTex"));
        }

        void drawBullets(const glm::mat4& cam, unsigned int numBullets,
                         const GL::Buffer& bulletPosList,
                         const GL::Texture2D& sprite)
        {
                glm::mat4 newCam;
                newCam[3] = cam[3];
                bulletProg.uniform("camera", cam);

                glDepthMask(GL_FALSE);
                glEnable(GL_BLEND);
                glActiveTexture(GL_TEXTURE0);
                sprite.bind();

                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glActiveTexture(GL_TEXTURE5);
                bulletPosTex.assign(GL_RG32F, bulletPosList);
                // bulletVao.bindVertexBuffer<glm::vec3>(0, quad->vertices, 0);
                quad->bind(bulletVao);
                quad->indices.bindElems();
                glDrawElementsInstanced(GL_TRIANGLES, numBullets*quad->size,
                                        GL_UNSIGNED_INT, nullptr, numBullets
                );
                glDisable(GL_BLEND);
                glDepthMask(GL_TRUE);
        }
};

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
