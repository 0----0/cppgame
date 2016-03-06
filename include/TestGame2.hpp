#pragma once

#include <memory>

class GeometryBuffer;
class Scene;
class Material;
class InputHandler;
class Renderer;
class Object;

class TestGame2Impl;

class TestGame2 {
private:
        std::unique_ptr<TestGame2Impl> impl;
public:
        TestGame2();
        TestGame2(TestGame2&& rhs);
        TestGame2& operator=(TestGame2&& rhs);
        ~TestGame2();

        void init();
        void update(InputHandler& input);
        void render(Renderer& renderer);
};
