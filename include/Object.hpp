#pragma once

#include "GeometryBuffer.hpp"
#include "Material.hpp"

#include <glm/gtx/transform.hpp>

#include <memory>

class Object {
public:
        std::shared_ptr<const GeometryBuffer> geometry;
        std::shared_ptr<Material> material;

        glm::mat4 objTransform;
        glm::vec3 velocity;
        glm::vec3 torque;

        Object(glm::mat4 objTransform, std::shared_ptr<const GeometryBuffer> geometry,
               std::shared_ptr<Material> material):
                geometry(std::move(geometry)),
                material(std::move(material)),
                objTransform(objTransform)
        {}

        glm::mat4 getTransform() const {
                return objTransform;
        }

        void update() {
                float strength = glm::length(torque);
                if (strength == 0.0f) return;
                glm::vec3 axis = torque / strength;

                objTransform = glm::rotate(objTransform, strength, axis);
                objTransform[3] += glm::vec4{velocity, 0.0f};

                float damp = 0.99f;
                torque *= damp;
                velocity *= damp;
        }
};
