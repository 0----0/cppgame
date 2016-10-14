#include "EnemyList.hpp"

#include "EnemyAI.hpp"
#include "BulletSystem.hpp"

void EnemyList::addEnemy(const Enemy& enemy) {
        for (int i = 0; i < positions.size(); i++) {
                if (!isAlive[i]) {
                        enemy.ai->id = i;
                        positions[i] = enemy.position;
                        aabbs[i] = enemy.bounds;
                        ais[i] = enemy.ai;
                        isAlive[i] = true;
                        return;
                }
        }
        enemy.ai->id = positions.size();
        positions.push_back(enemy.position);
        aabbs.push_back(enemy.bounds);
        ais.push_back(enemy.ai);
        isAlive.push_back(true);
}

void EnemyList::addEnemy(Enemy&& enemy) {
        for (int i = 0; i < positions.size(); i++) {
                if (!isAlive[i]) {
                        enemy.ai->id = i;
                        positions[i] = std::move(enemy.position);
                        aabbs[i] = std::move(enemy.bounds);
                        ais[i] = std::move(enemy.ai);
                        isAlive[i] = true;
                        return;
                }
        }
        enemy.ai->id = positions.size();
        positions.emplace_back(std::move(enemy.position));
        aabbs.emplace_back(std::move(enemy.bounds));
        ais.emplace_back(std::move(enemy.ai));
        isAlive.push_back(true);
}

void EnemyList::removeEnemy(unsigned int i) {
        ais[i] = nullptr;
        isAlive[i] = false;
}

void EnemyList::update(const EnemyWorldInformation& info) {
        for (int i = 0; i < ais.size(); i++) {
                if (!isAlive[i]) continue;
                ais[i]->update(*this, info);
        }
}

void EnemyList::testCollisions(BulletSystem& bullets, const EnemyWorldInformation& info) {
        AABB bulletBounds = bullets.bounds;
        for (glm::vec2& pos : bullets.bulletPos) {
                auto bounds = bulletBounds + pos;
                for (int i = 0; i < aabbs.size(); i++) {
                        if (!isAlive[i]) continue;
                        if (bounds.intersects(aabbs[i] + positions[i])) {
                                ais[i]->die(*this, info);
                        }
                }
        }
}
