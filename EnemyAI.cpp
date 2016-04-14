#include "EnemyAI.hpp"

#include "AssetManager.hpp"
#include "TestGame2.hpp"
#include "Scene.hpp"
#include "Object.hpp"
#include "EnemyList.hpp"
#include "PlayerController.hpp"

void EnemyAI::spawn(TestGame2& game) {
        auto geo = AssetManager::get().getModelBuffer("legobrick.ply");
        auto brickDiffuse = AssetManager::get().getImage("BrickTex.png");
        auto brickNormals = AssetManager::get().getImage("BrickNormals2.png");
        auto mat = std::make_shared<Material>(brickDiffuse, brickNormals);

        auto obj = std::make_shared<Object>(glm::mat4(), geo, mat);
        game.getScene().objects.push_back(obj);
        objWk = obj;
}
void EnemyAI::update(EnemyList& list) {
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
                // auto velocity = glm::vec2{0, -0.5};
                scene.bulletSystems[1]->addBullet({firePoint, velocity, 0});
        }
}

void EnemyAI::die(EnemyList& list) {
        if (objWk.expired()) return;
        auto obj = objWk.lock();
        
        list.game->getScene().removeObject(obj);

        list.removeEnemy(id);
}

EnemyAI::~EnemyAI() {

}
