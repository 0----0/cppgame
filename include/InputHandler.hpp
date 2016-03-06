#pragma once

#include "Keymap.hpp"

#include "GLFW/GLFW.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>

class InputHandler {
private:
        const GLFW::Window& window;
        glm::vec2 lastPos;
        bool mouseFree = false;
public:
        std::shared_ptr<Keymap> keymap;

        InputHandler(GLFW::Window& window, std::shared_ptr<Keymap> _keymap):
                window( window ),
                keymap( std::move(_keymap) )
        {
                ::glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                window.setFocusCallback([&](GLFW::Window& window, int focused) {
                        if (focused == GL_FALSE && !mouseFree) {
                                mouseFree = true;
                                ::glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        }
                });
                window.setMouseButtonCallback([&](GLFW::Window& window, int button, int action, int mods) {
                        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                                mouseFree = false;
                                ::glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                        }
                });
                window.setKeyCallback([&](GLFW::Window& window, int key, int scancode, int action, int mods) {
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

        bool getTrigger(const std::string& name) const {
                auto bind = keymap->getBind(name);
                return bind != -1 ? window.getKey(bind) : false;
        }

        glm::vec3 getMotionInput() const {
                glm::vec3 movement(0.0f);
                if (getTrigger("FORWARD")) {
                        movement += glm::vec3(0.0f, 1.0f, 0.0f);
                } if (getTrigger("BACK")) {
                        movement += glm::vec3(0.0f, -1.0f, 0.0f);
                } if (getTrigger("LEFT")) {
                        movement += glm::vec3(-1.0f, 0.0f, 0.0f);
                } if (getTrigger("RIGHT")) {
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

        bool getKey(int key) {
                return window.getKey(key);
        }
};
