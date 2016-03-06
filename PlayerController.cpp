#include "PlayerController.hpp"

#include "Object.hpp"
#include "InputHandler.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

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
