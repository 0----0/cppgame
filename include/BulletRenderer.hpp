#pragma once

#include "GL/GL.hpp"

#include "AssetManager.hpp"
#include "Assets/Geometry/GeometryBuffer.hpp"
#include "misc.hpp"

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class BulletRenderer {
public:
        GL::Program bulletProg;
        GL::VertexArray bulletVao;
        GL::TextureBuffer bulletPosTex;

        std::shared_ptr<Geometry> quad {
                AssetManager::get().getModel("quad.ply")
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

                bulletVao.vertexAttribFormat<glm::vec3>(bulletProg.attrib("vertPos"), 0);
                bulletVao.vertexAttribBinding(bulletProg.attrib("vertPos"), 0);
                bulletVao.enableVertexAttrib(bulletProg.attrib("vertPos"));

                bulletVao.vertexAttribFormat<glm::vec2>(bulletProg.attrib("vertTex"), 0);
                bulletVao.vertexAttribBinding(bulletProg.attrib("vertTex"), 2);
                bulletVao.enableVertexAttrib(bulletProg.attrib("vertTex"));
        }

        void updateProjection(const glm::mat4& projection) {
                bulletProg.uniform("projection", projection);
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
                quad->getBuffer().bind(bulletVao);
                quad->getBuffer().indices.bindElems();
                glDrawElementsInstanced(GL_TRIANGLES, numBullets*quad->getBuffer().size,
                                        GL_UNSIGNED_INT, nullptr, numBullets
                );
                glDisable(GL_BLEND);
                glDepthMask(GL_TRUE);
        }
};
