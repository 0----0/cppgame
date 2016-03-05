#pragma once

#include "GL/GL.hpp"

#include <memory>

struct Material {
        std::shared_ptr<const GL::Texture2D> diffuse;
        std::shared_ptr<const GL::Texture2D> normal;

        Material(std::shared_ptr<const GL::Texture2D> diffuse,
                 std::shared_ptr<const GL::Texture2D> normal):
                diffuse(std::move(diffuse)),
                normal(std::move(normal))
        {}

        void bind() {
                glActiveTexture(GL_TEXTURE0);
                diffuse->bind();
                glActiveTexture(GL_TEXTURE1);
                normal->bind();
        }
};
