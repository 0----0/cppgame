#pragma once

#include <memory>

class Object;
class TestGame2;
class EnemyList;

class EnemyAI {
public:
        unsigned int id;
        std::weak_ptr<Object> objWk;
        unsigned int lastFire;

        void spawn(TestGame2& game);
        void update(EnemyList& list);
        void die(EnemyList& list);
};
