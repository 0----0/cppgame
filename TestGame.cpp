#include "TestGame.hpp"

#include "AssetManager.hpp"
#include "GeometryBuffer.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Scene.hpp"

class TestGameImpl {
        std::unique_ptr<Scene> scene;
        std::shared_ptr<const GeometryBuffer> brickGeo;
        std::shared_ptr<const GeometryBuffer> shipGeo;

        std::shared_ptr<Material> brickMat;
        std::shared_ptr<Material> shipMat;

        std::shared_ptr<Object> ship;

        FiveDOFCamera camera;
        float sunRotation;
public:
        void init();

        void initAssets();
        void initScene();

        void update(InputHandler& input);

        void render(Renderer& renderer);
};

void TestGameImpl::initAssets() {
        brickGeo = AssetManager::get().getModelBuffer("legobrick.ply");
        shipGeo  = AssetManager::get().getModelBuffer("ship1 v9.ply");

        auto brickDiffuse = AssetManager::get().getImage("BrickTex.png");
        auto brickNormals = AssetManager::get().getImage("DefaultNormals.png");
        brickMat = std::make_shared<Material>(brickDiffuse, brickNormals);
        auto shipDiffuse = AssetManager::get().getImage("ship diffuse AO.png");
        auto shipNormals = AssetManager::get().getImage("ship normalsdf.png");
        shipMat = std::make_shared<Material>(shipDiffuse, shipNormals);
}

void TestGameImpl::initScene() {
        scene = std::make_unique<Scene>();
        scene->backgroundColor = glm::vec3(0.5f, 0.15f, 0.25f);
        scene->sceneDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        scene->sceneAmbient = glm::vec3(0.05, 0.1, 0.2);
        scene->sceneSpecular = glm::vec3(0.6, 0.5, 0.2f);

        const auto gridSize = 10;
        const auto gridSpacing = 2.f;
        for (int i = 0; i < gridSize; i++) { for (int j = 0; j < gridSize; j++) {
                float x = (i - gridSize/2) * gridSpacing;
                float z = (j + 1) * gridSpacing;
                auto obj = std::make_shared<Object>(glm::translate(glm::vec3(x, 0, z)), brickGeo, brickMat);
                scene->objects.push_back(std::move(obj));
        }}

        ship = std::make_unique<Object>(glm::mat4(), shipGeo, shipMat);
        ship->objTransform = glm::translate(glm::vec3{0, 10, 10});;
        scene->objects.push_back(ship);

        sunRotation = 0.0f;
        camera = FiveDOFCamera();
}

void TestGameImpl::init() {
        initAssets();
        initScene();
}

void TestGameImpl::update(InputHandler& input) {
        scene->update();

        scene->sunDirection = glm::vec3(glm::rotate(sunRotation,glm::vec3{0,1,0})*glm::vec4(glm::normalize(glm::vec3{-1,-1,-1}),1));
        if (input.getKey(GLFW_KEY_F)) {
                sunRotation += 0.01f;
        }

        camera.update(input);
}

void TestGameImpl::render(Renderer& renderer) {
        renderer.drawScene(camera.getMatrix(), *scene);
}

TestGame::TestGame(): impl (std::make_unique<TestGameImpl>()) {}
TestGame::TestGame(TestGame&& rhs) = default;
TestGame& TestGame::operator=(TestGame&& rhs) = default;
TestGame::~TestGame() = default;

void TestGame::init() { impl->init(); }
void TestGame::update(InputHandler& input) { impl->update(input); }
void TestGame::render(Renderer& renderer) { impl->render(renderer); }
