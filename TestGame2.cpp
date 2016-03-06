#include "GL/GL.hpp"
#include "TestGame2.hpp"

#include "AssetManager.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "InputHandler.hpp"
#include "PlayerController.hpp"

struct Bullet {
        glm::vec2 pos;
        glm::vec2 vel;
};

class BulletSystem {
private:
        std::vector<glm::vec2> bulletPos;
        std::vector<glm::vec2> bulletVel;
        std::vector<unsigned int> bulletDeathTime;
        GL::Buffer bulletPosBuff;

        std::shared_ptr<GL::Texture2D> img;
public:
        void addBullet() {
                bulletPos.push_back({0, 0});
                bulletVel.push_back({0, 0});
                bulletDeathTime.push_back(0);
        }
        void update() {
                for (int i = 0; i < bulletPos.size(); i++) {
                        bulletPos[i] += bulletVel[i];
                }
        }
};

class TestGame2Impl {
        std::unique_ptr<Scene> scene;
        std::shared_ptr<const GeometryBuffer> brickGeo;
        std::shared_ptr<const GeometryBuffer> shipGeo;

        std::shared_ptr<Material> brickMat;
        std::shared_ptr<Material> shipMat;

        std::shared_ptr<Object> ship;

        TopDownPlayerController player;

        float sunRotation;
public:
        void init();

        void initAssets();
        void initScene();

        void update(InputHandler& input);

        void render(Renderer& renderer);
};

void TestGame2Impl::initAssets() {
        brickGeo = AssetManager::get().getModelBuffer("legobrick.ply");
        shipGeo  = AssetManager::get().getModelBuffer("ship1 v3.ply");

        auto brickDiffuse = AssetManager::get().getImage("BrickTex.png");
        auto brickNormals = AssetManager::get().getImage("BrickNormals2.png");
        brickMat = std::make_shared<Material>(brickDiffuse, brickNormals);
        shipMat = brickMat;
}

void TestGame2Impl::initScene() {
        scene = std::make_unique<Scene>();
        scene->backgroundColor = glm::vec3(0.5f, 0.15f, 0.25f);

        const auto gridSize = 10;
        const auto gridSpacing = 2.f;
        for (int i = 0; i < gridSize; i++) { for (int j = 0; j < gridSize; j++) {
                float x = (i - gridSize/2) * gridSpacing;
                float z = (j + 1) * gridSpacing;
                auto obj = std::make_shared<Object>(glm::translate(glm::vec3(x, -10, z)), brickGeo, brickMat);
                scene->objects.push_back(std::move(obj));
        }}

        ship = std::make_unique<Object>(glm::mat4(), shipGeo, shipMat);
        scene->objects.push_back(ship);

        sunRotation = 0.0f;

        player.objWk = ship;
}

void TestGame2Impl::init() {
        initAssets();
        initScene();
}

void TestGame2Impl::update(InputHandler& input) {
        scene->update();

        scene->sunDirection = glm::vec3(glm::rotate(sunRotation,glm::vec3{0,1,0})*glm::vec4(glm::normalize(glm::vec3{-1,-1,-1}),1));
        if (input.getKey(GLFW_KEY_F)) {
                sunRotation += 0.01f;
        }

        player.update(input);
}

void TestGame2Impl::render(Renderer& renderer) {
        renderer.drawScene(player.getMatrix(), *scene);
}

TestGame2::TestGame2(): impl( std::make_unique<TestGame2Impl>() ) {}
TestGame2::TestGame2(TestGame2&& rhs) = default;
TestGame2& TestGame2::operator=(TestGame2&& rhs) = default;
TestGame2::~TestGame2() = default;

void TestGame2::init() { impl->init(); }
void TestGame2::update(InputHandler& input) { impl->update(input); }
void TestGame2::render(Renderer& renderer) { impl->render(renderer); }
