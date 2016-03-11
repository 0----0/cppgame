#pragma once

#include <memory>

class GeometryBuffer;
class Scene;
class Material;
class InputHandler;
class Renderer;
class Object;

class TestGame2;

class TestGame2Wrp {
private:
        std::unique_ptr<TestGame2> impl;
public:
        TestGame2Wrp();
        TestGame2Wrp(TestGame2Wrp&& rhs);
        TestGame2Wrp& operator=(TestGame2Wrp&& rhs);
        ~TestGame2Wrp();

        void init();
        void update(InputHandler& input);
        void render(Renderer& renderer);
        Scene& getScene();
};
