#pragma once

#include <memory>

class InputHandler;
class Renderer;

class TestGameImpl;

class TestGame {
        std::unique_ptr<TestGameImpl> impl;
public:
        TestGame();
        TestGame(TestGame&& rhs);
        TestGame& operator=(TestGame&& rhs);
        ~TestGame();

        void init();
        void update(InputHandler& input);
        void render(Renderer& renderer);
};
