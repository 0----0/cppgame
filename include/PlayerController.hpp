#include <memory>
#include <glm/glm.hpp>

class Object;
class InputHandler;

class PlayerController {
public:
        std::weak_ptr<Object> objWk;
        void update(InputHandler& input);
        glm::mat4 getMatrix();
};

class TopDownPlayerController {
public:
        std::weak_ptr<Object> objWk;
        glm::vec3 position{0, 24, 0};

        void update(InputHandler& input);
        glm::mat4 getMatrix();
};
