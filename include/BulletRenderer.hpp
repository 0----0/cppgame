#pragma once

#include "GL/GL.hpp"

#include "AssetManager.hpp"
#include "GeometryBuffer.hpp"
#include "misc.hpp"

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

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

                bulletProg.uniform("projection", glm::perspectiveFovRH(3.14159f/4.0f, 1024.0f, 768.0f, 0.01f, 128.0f));

                bulletVao.vertexAttribFormat<glm::vec3>(bulletProg.attrib("vertPos"), 0);
                bulletVao.vertexAttribBinding(bulletProg.attrib("vertPos"), 0);
                bulletVao.enableVertexAttrib(bulletProg.attrib("vertPos"));

                bulletVao.vertexAttribFormat<glm::vec2>(bulletProg.attrib("vertTex"), 0);
                bulletVao.vertexAttribBinding(bulletProg.attrib("vertTex"), 2);
                bulletVao.enableVertexAttrib(bulletProg.attrib("vertTex"));
        }

        void drawBullets(const glm::mat4& cam, unsigned int numBullets,
                         const GL::Buffer& bulletPosList,
                         const GL::Texture2D& sprite, glm::vec2 scale)
        {
                glm::mat4 newCam;
                newCam[3] = cam[3];
                bulletProg.uniform("camera", cam);
                bulletProg.uniform("scale", scale);

                glDepthMask(GL_FALSE);
                glEnable(GL_BLEND);
                glActiveTexture(GL_TEXTURE0);
                sprite.bind();

                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glActiveTexture(GL_TEXTURE5);
                bulletPosTex.assign(GL_RG32F, bulletPosList);
                quad->bind(bulletVao);
                quad->indices.bindElems();
                glDrawElementsInstanced(GL_TRIANGLES, numBullets*quad->size,
                                        GL_UNSIGNED_INT, nullptr, numBullets
                );
                glDisable(GL_BLEND);
                glDepthMask(GL_TRUE);
        }
};
