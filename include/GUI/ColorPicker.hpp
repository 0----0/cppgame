#pragma once

#include <string>
#include <glm/glm.hpp>
#include "nuklear/nuklear.h"
#include "GUI/INKControl.hpp"

class ColorPicker : public INKControl {
private:
        static auto nk_color_from_vec3(glm::vec3 col) { col *= 255.0f; return nk_rgb(col.r, col.g, col.b); }
        static auto vec3_from_nk_color(nk_color col) { return glm::vec3(col.r, col.g, col.b)/255.0f; }
public:
        std::string name{"Color"};
        struct nk_color color{nk_rgb(0,0,0)};

        ColorPicker(std::string name, nk_color color):
                name(name),
                color(color)
        {}

        void draw(nk_context* ctx) override;

        struct nk_color getColorNK() { return color; }
        glm::vec3 getColorVec3() { return vec3_from_nk_color(color); }

        void setColor(struct nk_color col) {
                color = col;
        }

        void setColor(glm::vec3 col) {
                color = nk_color_from_vec3(col);
        }
};
