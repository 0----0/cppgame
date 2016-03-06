#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "InputHandler.hpp"

class SixDOFCamera {
private:
        glm::vec3 position;
        glm::mat4 rotation;
public:
        void update(const InputHandler& input) {
                glm::vec3 movement = input.getMotionInput() * 0.2f;
                rotation = input.get6DOFRotationInput() * rotation;
                movement = glm::vec3(glm::inverse(rotation) * glm::vec4(movement, 0.0f));
                position += movement;
        }

        glm::mat4 getMatrix() {
                return glm::translate(rotation, -position);
        }
};

class SixDOFCamera2 {
private:
        glm::mat4 camera;
public:
        void update(const InputHandler& input) {
                camera = input.get6DOFRotationInput() * camera;

                auto motion = input.getMotionInput() * 0.2f;
                auto inv = glm::inverse(camera);
                auto worldMotion = inv * glm::vec4(-motion,1) - inv * glm::vec4(glm::vec3(0),1);
                camera = glm::translate(camera, glm::vec3{worldMotion});
        }

        glm::mat4 getMatrix() {
                return camera;
        }
};

class FiveDOFCamera {
private:
        glm::vec3 position;
        glm::vec2 rotation;

        glm::mat4 matrix; // cached
public:
        void update(const InputHandler& input) {
                rotation += input.getRotationInput();
                rotation.y = glm::clamp(rotation.y, (float)-M_PI/2.0f, (float)M_PI/2.0f);

                matrix = glm::rotate(rotation.y, glm::vec3(1.0, 0.0, 0.0));
                matrix = glm::rotate(matrix, rotation.x, glm::vec3(0.0, 1.0, 0.0));

                glm::vec3 movement = input.getMotionInput() * 0.2f;

                movement = glm::vec3(glm::inverse(matrix) * glm::vec4(movement, 0.0f));
                position += movement;

                matrix = glm::translate(matrix, -position);
        }

        glm::mat4 getMatrix() {
                return matrix;
        }
};
