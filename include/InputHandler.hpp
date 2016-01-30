#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "GLFW/GLFW.hpp"

class InputHandler {
private:
        const GLFW::Window& window;
        glm::vec2 lastPos;
        bool mouseFree = false;
public:
        InputHandler(GLFW::Window& window): window(window) {
                ::glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                // ::glfwSetWindowFocusCallback(window(), focusCallback);
                window.setFocusCallback([&](GLFW::Window&, int focused) {
                        if (focused == GL_FALSE && !mouseFree) {
                                mouseFree = true;
                                ::glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        }
                });
                window.setMouseButtonCallback([&](GLFW::Window&, int button, int action, int mods) {
                        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                                mouseFree = false;
                                ::glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                        }
                });
                window.setKeyCallback([&](GLFW::Window&, int key, int scancode, int action, int mods) {
                        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && !mouseFree) {
                                mouseFree = true;
                                ::glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        }
                });
        }

        void update() {
                lastPos = window.getCursorPos();
                glfwPollEvents();
        }

        glm::vec3 getMotionInput() const {
                glm::vec3 movement(0.0f);
                if (window.getKey(GLFW_KEY_W)) {
                        movement += glm::vec3(0.0f, 1.0f, 0.0f);
                } if (window.getKey(GLFW_KEY_S)) {
                        movement += glm::vec3(0.0f, -1.0f, 0.0f);
                } if (window.getKey(GLFW_KEY_A)) {
                        movement += glm::vec3(-1.0f, 0.0f, 0.0f);
                } if (window.getKey(GLFW_KEY_D)) {
                        movement += glm::vec3(1.0f, 0.0f, 0.0f);
                }
                std::swap(movement.y, movement.z);
                movement.z *= -1.0f;
                return movement;
        }

        glm::vec2 getRotationInput() const {
                glm::vec2 curPos = window.getCursorPos();
                glm::vec2 mouseMovement = curPos - lastPos;
                if (mouseFree) mouseMovement = {0,0};

                glm::vec2 keyboardRotation;
                if (window.getKey(GLFW_KEY_LEFT)) {
                        keyboardRotation.x -= 1.0f;
                } if (window.getKey(GLFW_KEY_RIGHT)) {
                        keyboardRotation.x += 1.0f;
                } if (window.getKey(GLFW_KEY_UP)) {
                        keyboardRotation.y -= 1.0f;
                } if (window.getKey(GLFW_KEY_DOWN)) {
                        keyboardRotation.y += 1.0f;
                }

                return mouseMovement * 0.01f + keyboardRotation * 0.1f;
        }

        glm::vec3 get3DOFRotationInput() const {
                glm::vec2 mouse = getRotationInput();
                glm::vec3 rotation;
                if (!getRightMouse()) {
                        rotation += mouse.x * glm::vec3(0.0f, -1.0f, 0.0f);
                } else {
                        rotation += mouse.x * glm::vec3(0.0f, 0.0f, 1.0f);
                }
                rotation += mouse.y * glm::vec3(1.0f, 0.0f, 0.0f);
                return rotation;
        }

        glm::mat4 get6DOFRotationInput() const {
                glm::vec2 mouse = getRotationInput();
                glm::mat4 rotation;
                if (!getRightMouse()) {
                        rotation = glm::rotate(rotation, mouse.x, glm::vec3( 0.0f,  1.0f, 0.0f));
                } else {
                        rotation = glm::rotate(rotation, mouse.x, glm::vec3(0.0f, 0.0f, 1.0f));
                }
                rotation = glm::rotate(rotation, mouse.y, glm::vec3( 1.0f,  0.0f, 0.0f));
                return rotation;
        }

        bool getLeftMouse() const {
                return window.getMouseButton(GLFW_MOUSE_BUTTON_LEFT);
        }

        bool getRightMouse() const {
                return window.getMouseButton(GLFW_MOUSE_BUTTON_RIGHT);
        }
};
