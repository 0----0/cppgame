#include "GLFW/GLFW.hpp"
#include "GL/GL.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/projection.hpp>

#include "Object.hpp"
#include "Scene.hpp"
#include "misc.hpp"

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
                shadowTex.assign(0, GL_DEPTH_COMPONENT32, xRes, yRes);
                float borderColor[4]{1.0f, 1.0f, 1.0f, 1.0f};
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

class Renderer {
public:
        GLFW::Window glfwWindow{initWindow()};
        GL::Program renderProgram{initProgram()};
        GL::VertexArray vao{initVertexArray(renderProgram)};

        GL::Program shadowProg;
        GL::VertexArray shadowVao;
        Shadowmap shadowmap{1024, 1024, 32.0f, 256.0f};
        Shadowmap shadowmap2{1024, 1024, 64.0f, 256.0f};
        ShadowmapArray shadowmaps {1024, 1024, {8, 16, 32, 64, 128, 256}, 256};

        static GLFW::Window initWindow() {
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
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
                auto vert = GL::Shader::fromString(GL_VERTEX_SHADER, readFile("../shadows_vert.glsl"));
                shadowProg.attachShader(vert);
                shadowProg.link();
                shadowProg.use();

                shadowVao.vertexAttribFormat<glm::vec3>(shadowProg.attrib("vertPos"), 0);
                shadowVao.vertexAttribBinding(shadowProg.attrib("vertPos"), 0);
                shadowVao.enableVertexAttrib(shadowProg.attrib("vertPos"));
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
                // renderProgram.uniform<int>("shadowMapID", 4);
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

        void drawShadowmaps(glm::vec3 cameraPos, const Scene& scene, ShadowmapArray& shMaps) {
                glm::vec3 l = scene.sunDirection;

                glViewport(0, 0, shMaps.xRes, shMaps.yRes);
                for (int i = 0; i < shMaps.numShadowmaps(); ++i) {
                        shadowProg.uniform("projView", shMaps.projView(i, cameraPos, l));
                        shMaps.shadowFbuffs[i].bind();
                        glClear(GL_DEPTH_BUFFER_BIT);
                        for(auto& obj : scene.objects) {
                                drawObjectShadow(*obj);
                        }
                }
        }

        void drawScene(const glm::mat4& camera, const Scene& scene) {
                glm::vec3 cameraPos { glm::inverse(camera) * glm::vec4(0,0,0,1) };

                // drawShadowmap(cameraPos, scene, shadowmap);
                // drawShadowmap(cameraPos, scene, shadowmap2);
                drawShadowmaps(cameraPos, scene, shadowmaps);

                renderProgram.use();
                renderProgram.uniform("camera", camera);
                renderProgram.uniform("cameraPos", cameraPos);
                renderProgram.uniform("lightDirection", scene.sunDirection);

                glActiveTexture(GL_TEXTURE4);
                // shadowmap.shadowTex.bind();
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
                // glCullFace(GL_BACK);

                for (auto& obj : scene.objects) {
                        drawObject(*obj);
                }

                glfwWindow.swapBuffers();
        }
};
