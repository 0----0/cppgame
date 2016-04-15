#pragma once

#include "AxisAlignedBoundingBox.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

struct EnemyWorldInformation;
class EnemyAI;
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
        void addEnemy(const Enemy& enemy);
        void addEnemy(Enemy&& enemy);
        void removeEnemy(unsigned int i);

        void update(const EnemyWorldInformation& info);

        void testCollisions(BulletSystem& bullets, const EnemyWorldInformation& info);
};
