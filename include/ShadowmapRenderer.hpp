#pragma once

#include "GL/GL.hpp"

#include "Object.hpp"
#include "Scene.hpp"

#include "misc.hpp"

#include <glm/gtx/transform.hpp>

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
                shadowVao.bindVertexBuffer<glm::vec3>(0, obj.geometry->getBuffer().vertices, 0);
                obj.geometry->getBuffer().draw();
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
