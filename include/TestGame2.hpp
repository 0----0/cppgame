#include <vector>
#include <memory>

class Scene;
class ShipController;
class TestGame2Wrp;
class InputHandler;
class Renderer;
class EnemyList;
class LuaInterface;
class PreferenceWindow;

class TestGame2 {
        std::shared_ptr<Scene> scene;
        std::unique_ptr<LuaInterface> lua;

        std::unique_ptr<EnemyList> enemies;
        std::unique_ptr<ShipController> player;

        std::shared_ptr<PreferenceWindow> prefs;

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
