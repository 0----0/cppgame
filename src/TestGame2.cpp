#include "GL/GL.hpp"
#include "TestGame2Wrp.hpp"
#include "TestGame2.hpp"

#include "AssetManager.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "InputHandler.hpp"
#include "PlayerController.hpp"
#include "BulletSystem.hpp"
#include "EnemyAI.hpp"
#include "TriShotEnemyAI.hpp"
#include "EnemyList.hpp"
#include "LuaInterface.hpp"
#include "Objects.hpp"

void TestGame2::initAssets() {
}

glm::vec3 parseVec3(int intForm) {
        int r = (intForm >> 16) & 0xFF;
        int g = (intForm >> 8)  & 0xFF;
        int b = intForm & 0xFF;
        return {r/255.0f, g/255.0f, b/255.0f};
}

void TestGame2::initScene() {
        lua = std::make_unique<LuaInterface>();
        auto ship = std::make_shared<Object>(Obj::Player::ship());
        scene = std::make_unique<Scene>();
        scene->backgroundColor = parseVec3(0x06070A);
        scene->sceneAmbient = parseVec3(0x172B38);
        scene->sceneSpecular = glm::vec3(0.6, 0.5, 0.2f);
        scene->sceneDiffuse = parseVec3(0x9EC2FF);


        auto brickGeo = AssetManager::get().getModel("legobrick.ply");
        auto brickDiffuse = AssetManager::get().getTexture("BrickTex.png");
        auto brickNormals = AssetManager::get().getTexture("BrickNormals2.png");
        auto brickMat = std::make_shared<Material>(brickDiffuse, brickNormals);

        const auto gridSize = 10;
        const auto gridSpacing = 2.f;
        for (int i = 0; i < gridSize; i++) { for (int j = 0; j < gridSize; j++) {
                float x = (i - gridSize/2 + 0.5f) * gridSpacing;
                float z = (j + 1 + 0.5f) * gridSpacing;
                auto obj = std::make_shared<Object>(glm::translate(glm::vec3(x, -10, z-10)), brickGeo, brickMat);
                scene->objects.push_back(std::move(obj));
        }}

        scene->objects.push_back(ship);

        sunRotation = 0.0f;

        player = std::make_unique<ShipController>();
        player->objWk = ship;

        auto img = AssetManager::get().getTexture("bullet2.png");
        auto bulletSystem = std::make_unique<BulletSystem>(
                std::move(img), glm::vec2{0.25f, 4.0f}
        );
        scene->bulletSystems.emplace_back(std::move(bulletSystem));

        auto img2 = AssetManager::get().getTexture("bullet1.png");
        auto bulletSystem2 = std::make_unique<BulletSystem>(
                std::move(img2), glm::vec2{1.0f, 1.0f}
        );
        scene->bulletSystems.emplace_back(std::move(bulletSystem2));

        enemies = std::make_unique<EnemyList>();
}

struct Boop {
        int boop() const {
                return 2;
        }

        static void pushLuaMethods(LuaInterface& lua) {
                lua.setMethod<Boop, decltype(&Boop::boop), &Boop::boop>("boop");
        }
};

void TestGame2::initLua() {
        lua->addGlobalFn("setSceneAmbient", [&](float r, float g, float b) {
                scene->sceneAmbient = glm::vec3(r, g, b);
        });

        lua->addGlobalFn("setSceneSpecular", [&](float r, float g, float b) {
                scene->sceneSpecular = glm::vec3(r, g, b);
        });

        lua->addGlobalFn("setSceneDiffuse", [&](float r, float g, float b) {
                scene->sceneDiffuse = glm::vec3(r, g, b);
        });

        lua->setObj("boop", Boop());

        lua->loadFile("scene.lua");
}

void TestGame2::init() {
        initAssets();
        initScene();
        initLua();
}

void TestGame2::updateLevel() {
        struct EnemyWorldInformation info {
                .scene = *this->scene,
                .player = *player
        };

        if (scene->ticks == 0) {
                Enemy enemy {
                        .position = {32.f, 16.f},
                        .bounds = {{-1.0f, -1.0f}, {1.0f, 1.0f}},
                        .ai = std::make_shared<EnemyAI>()
                };
                enemy.ai->spawn(info);
                enemies->addEnemy(std::move(enemy));
        } else if (scene->ticks == 60) {
                Enemy enemy {
                        .position = {32.f, 8.f},
                        .bounds = {{-1.0f, -1.0f}, {1.0f, 1.0f}},
                        .ai = std::make_shared<TriShotEnemyAI>()
                };
                enemy.ai->spawn(info);
                enemies->addEnemy(std::move(enemy));
        } else if (scene->ticks == 120) {
                Enemy enemy {
                        .position = {32.f, 0.f},
                        .bounds = {{-1.0f, -1.0f}, {1.0f, 1.0f}},
                        .ai = std::make_shared<EnemyAI>()
                };
                enemy.ai->spawn(info);
                enemies->addEnemy(std::move(enemy));
        }

}

void TestGame2::update(InputHandler& input) {
        updateLevel();
        scene->update();

        scene->sunDirection = glm::vec3(glm::rotate(sunRotation,glm::vec3{0,1,0})*glm::vec4(glm::normalize(glm::vec3{-1,-1,-1}),1));
        if (input.getKey(GLFW_KEY_F)) {
                sunRotation += 0.01f;
        }

        player->update(input, *scene);

        auto playerBounds = AABB{{-0.5f, -0.5f}, {0.5f, 0.5f}};
        playerBounds = playerBounds + player->getPosition();
        auto& bullets = *scene->bulletSystems[1];
        auto& bounds = bullets.bounds;
        for (auto& p : bullets.bulletPos) {
                if (playerBounds.intersects(bounds + p)) {
                        init();
                        update(input);
                        return;
                }
        }

        struct EnemyWorldInformation info {
                .scene = *this->scene,
                .player = *player
        };

        enemies->update(info);
        enemies->testCollisions(*scene->bulletSystems[0], info);
}

void TestGame2::render(Renderer& renderer) {
        renderer.drawScene(player->getMatrix(), *scene);
}

TestGame2Wrp::TestGame2Wrp(): impl ( std::make_unique<TestGame2>() ) {}
TestGame2Wrp::TestGame2Wrp(TestGame2Wrp&& rhs) = default;
TestGame2Wrp& TestGame2Wrp::operator=(TestGame2Wrp&& rhs) = default;
TestGame2Wrp::~TestGame2Wrp() = default;

void TestGame2Wrp::init() { impl->init(); }
void TestGame2Wrp::update(InputHandler& input) { impl->update(input); }
void TestGame2Wrp::render(Renderer& renderer) { impl->render(renderer); }
Scene& TestGame2Wrp::getScene() { return *impl->scene; }