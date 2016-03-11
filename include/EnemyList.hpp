#pragma once

#include "AxisAlignedBoundingBox.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

class EnemyAI;
class TestGame2;
class BulletSystem;

struct Enemy {
        glm::vec2 position;
        AABB bounds;
        std::shared_ptr<EnemyAI> ai;
};

class EnemyList {
        std::vector<AABB> aabbs;
        std::vector<std::shared_ptr<EnemyAI>> ais;
        std::vector<bool> isAlive;

public:
        std::vector<glm::vec2> positions;
        TestGame2* game = nullptr;
        void addEnemy(const Enemy& enemy);
        void addEnemy(Enemy&& enemy);
        void removeEnemy(unsigned int i);

        void update(TestGame2& game);

        void testCollisions(BulletSystem& bullets);
};
