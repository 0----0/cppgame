#pragma once

#include "Object.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

class Scene {
public:
        glm::vec3 backgroundColor;
        glm::vec3 sunDirection;
        std::vector<std::shared_ptr<Object>> objects;

        void update() {
                for(auto& obj : objects) {
                        obj->update();
                }
        }
};
