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
class LuaInterface;

class TestGame2 {
        std::unique_ptr<Scene> scene;
        std::unique_ptr<LuaInterface> lua;

        std::unique_ptr<EnemyList> enemies;
        std::unique_ptr<ShipController> player;

        float sunRotation;
        float sunElevation;

        void updateLevel();

        friend class TestGame2Wrp;
public:

        void init();

        void initAssets();
        void initScene();
        void initLua();

        void update(InputHandler& input);

        void render(Renderer& renderer);
        Scene& getScene() {
                return *scene;
        }

};
