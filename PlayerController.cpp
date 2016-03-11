#include "PlayerController.hpp"

#include "Scene.hpp"
#include "Object.hpp"
#include "InputHandler.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

glm::mat4 getLockedCamera(glm::vec3 offset, const Object& obj) {
        auto view = glm::inverse(obj.objTransform);
        view = glm::rotate((float)M_PI, glm::vec3{0, 1, 0}) * view;
        view[3] += glm::vec4{offset, 0};
        return view;
}

void PlayerController::update(InputHandler& input) {
        if (objWk.expired()) return;
        auto obj = objWk.lock();

        auto camMatrix = glm::rotate((float)M_PI, glm::vec3{0, 1, 0});
        auto orientationMatrix = obj->objTransform * camMatrix;
        auto motion = glm::vec4(input.getMotionInput(), 0.f);

        obj->velocity += glm::vec3(orientationMatrix * motion) * 0.005f;
        obj->torque += input.get3DOFRotationInput() * 0.02f;
}

glm::mat4 PlayerController::getMatrix() {
        if (objWk.expired()) return glm::mat4();
        auto obj = objWk.lock();

        return getLockedCamera(glm::vec3(0, -4, -12), *obj);
}



glm::mat4 getLockedOrbitCamera(glm::vec3 offset, const Object& obj) {
        auto pos = glm::vec3(obj.getTransform() * glm::vec4(0, 0, 0, 1));
        auto up = glm::vec3(0, 0, 1);
        return glm::lookAt(pos + offset, pos, up);
}




glm::mat4 getTopDownCamera(glm::vec3 offset, const Object& obj) {
        auto pos = glm::vec3(obj.getTransform() * glm::vec4(0, 0, 0, 1));
        auto forward = glm::vec3(obj.getTransform() * glm::vec4(0, 0, 1, 0));
        forward *= glm::vec3(1, 0, 1);
        return glm::lookAt(pos + offset, pos, forward);
}

glm::mat4 getFixedTopDownCamera(glm::vec3 offset, const Object& obj) {
        auto forward = glm::vec3(obj.getTransform() * glm::vec4(0, 0, 1, 0));
        forward *= glm::vec3(1, 0, 1);
        return glm::lookAt(offset, glm::vec3(0, 0, 0), forward);
}

void TopDownPlayerController::update(InputHandler& input) {
        if (objWk.expired()) return;
        auto obj = objWk.lock();

        auto camMatrix = glm::rotate((float)M_PI, glm::vec3{0, 1, 0});
        auto orientationMatrix = obj->objTransform * camMatrix;
        auto motion = glm::vec4(input.getMotionInput(), 0.f);

        auto worldMotion = glm::vec3(orientationMatrix * motion) * 0.2f;
        obj->objTransform[3] += glm::vec4(worldMotion, 0.0f);
}

glm::mat4 TopDownPlayerController::getMatrix() {
        if (objWk.expired()) return glm::mat4();
        auto obj = objWk.lock();

        return getFixedTopDownCamera(position, *obj);
}

void ShipController::update(InputHandler& input, Scene& scene) {
        if (objWk.expired()) return;
        auto obj = objWk.lock();
        TopDownPlayerController::update(input);

        if (input.getTrigger("FIRE") && scene.ticks - lastFire > 2) {
                lastFire = scene.ticks;
                auto objPos = glm::vec3(obj->getTransform() * glm::vec4(0, 0, 0, 1));
                auto objPos2d = glm::vec2(objPos.x, objPos.z);
                auto firePoint1 = objPos2d + glm::vec2{ 3.6, 3};
                auto firePoint2 = objPos2d + glm::vec2{-3.6, 3};
                auto firePoint3 = objPos2d + glm::vec2{ 1.9, 3.2};
                auto firePoint4 = objPos2d + glm::vec2{-1.9, 3.2};
                auto velocity = glm::vec2{0, 1.5};
                scene.bulletSystems[0]->addBullet({firePoint1, velocity, 0});
                scene.bulletSystems[0]->addBullet({firePoint2, velocity, 0});
                scene.bulletSystems[0]->addBullet({firePoint3, velocity, 0});
                scene.bulletSystems[0]->addBullet({firePoint4, velocity, 0});
        }
}

glm::vec2 ShipController::getPosition() const {
        if (objWk.expired()) return {};
        auto obj = objWk.lock();

        glm::vec3 objPos(obj->objTransform * glm::vec4{0,0,0,1});
        return {objPos.x, objPos.z};
}
