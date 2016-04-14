#include "TriShotEnemyAI.hpp"
#include "EnemyList.hpp"
#include "TestGame2.hpp"
#include "Object.hpp"
#include "Scene.hpp"
#include "PlayerController.hpp"

#include <glm/gtx/rotate_vector.hpp>

void TriShotEnemyAI::update(EnemyList& list) {
        if (objWk.expired()) return;
        auto obj = objWk.lock();

        auto& game = *list.game;
        auto& scene = game.getScene();


        list.positions[id].x -= 0.15f;
        obj->objTransform[3].x = list.positions[id].x;
        obj->objTransform[3].z = list.positions[id].y;

        if (scene.ticks - lastFire > 30) {
                lastFire = scene.ticks;
                auto objPos = glm::vec3(obj->getTransform() * glm::vec4(0, 0, 0, 1));
                auto objPos2d = glm::vec2(objPos.x, objPos.z);
                auto firePoint = objPos2d;

                auto playerPos = game.player->getPosition();
                auto velocity = glm::normalize(playerPos - objPos2d)*0.25f;
                auto velocity2 = glm::rotate(velocity, (float)M_PI/16.0f);
                auto velocity3 = glm::rotate(velocity, -(float)M_PI/16.0f);
                scene.bulletSystems[1]->addBullet({firePoint, velocity2, 0});
                scene.bulletSystems[1]->addBullet({firePoint, velocity, 0});
                scene.bulletSystems[1]->addBullet({firePoint, velocity3, 0});
        }
}

TriShotEnemyAI::~TriShotEnemyAI() {

}
