#include <memory>
#include <glm/glm.hpp>

class Object;
class InputHandler;
class Scene;

class PlayerController {
public:
        std::weak_ptr<Object> objWk;
        void update(InputHandler& input);
        glm::mat4 getMatrix();
};

class TopDownPlayerController {
public:
        std::weak_ptr<Object> objWk;
        glm::vec3 position{0, 48, 0};

        void update(InputHandler& input);
        glm::mat4 getMatrix();
};

class ShipController : public TopDownPlayerController {
protected:
        unsigned int lastFire = 0;
public:
        void update(InputHandler& input, Scene& scene);
        glm::vec2 getPosition() const;
};
