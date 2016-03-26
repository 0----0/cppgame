#pragma once

#include "Object.hpp"
#include "BulletSystem.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <list>
#include <memory>

namespace GL { class Texture2D; }

class Scene {
public:
        glm::vec3 backgroundColor;
        std::shared_ptr<GL::Texture2D> background;

        glm::vec3 sceneAmbient;
        glm::vec3 sceneDiffuse;
        glm::vec3 sceneSpecular;

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

        void removeObject(std::shared_ptr<Object> obj) {
                for (int i = 0; i < objects.size(); i++) {
                        if (objects[i] == obj) {
                                objects[i] = std::move(objects[objects.size() - 1]);
                                objects.resize(objects.size() - 1);
                                return;
                        }
                }
        }
};
