#include <vector>
#include <memory>

class Scene;
class GeometryBuffer;
class Material;
class Object;
class ShipController;
class EnemyAI;
class BulletSystem;
class TestGame2Wrp;
class InputHandler;
class Renderer;
class EnemyList;

class TestGame2 {
        std::unique_ptr<Scene> scene;
        std::shared_ptr<const GeometryBuffer> brickGeo;
        std::shared_ptr<const GeometryBuffer> shipGeo;

        std::shared_ptr<Material> brickMat;
        std::shared_ptr<Material> shipMat;

        std::shared_ptr<Object> ship;

        std::vector<std::unique_ptr<EnemyAI>> enemyAIs;
        std::unique_ptr<EnemyList> enemies;

        std::shared_ptr<BulletSystem> playerBulletSystem;
        std::shared_ptr<BulletSystem> enemyBulletSystem;

        float sunRotation;

        void updateLevel();

        friend class TestGame2Wrp;
public:
        std::unique_ptr<ShipController> player;

        void init();

        void initAssets();
        void initScene();

        void update(InputHandler& input);

        void render(Renderer& renderer);
        Scene& getScene() {
                return *scene;
        }

};
