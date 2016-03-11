#pragma once

#include "Object.hpp"
#include "BulletSystem.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace GL { class Texture2D; }

class Scene {
public:
        glm::vec3 backgroundColor;
        std::shared_ptr<GL::Texture2D> background;

        glm::vec3 sunDirection;
        std::vector<std::shared_ptr<Object>> objects;
        std::vector<std::shared_ptr<BulletSystem>> bulletSystems;

        unsigned int ticks = 0;

        void update() {
                for(auto& obj : objects) {
                        obj->update();
                }
                for(auto& b : bulletSystems) {
                        b->update();
                }
                ticks++;
        }
};
