#pragma once

#include <memory>

class Object;
class TestGame2;
class EnemyList;

class EnemyAI {
protected:
        std::weak_ptr<Object> objWk;
        unsigned int lastFire;
public:
        unsigned int id;

        virtual void spawn(TestGame2& game);
        virtual void update(EnemyList& list);
        virtual void die(EnemyList& list);

        virtual ~EnemyAI();
};
