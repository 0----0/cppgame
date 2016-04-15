#pragma once

#include <memory>

class Object;
class TestGame2;
class EnemyList;
class Scene;
class ShipController;

struct EnemyWorldInformation {
        Scene& scene;
        ShipController& player;
};

class EnemyAI {
protected:
        std::weak_ptr<Object> objWk;
        unsigned int lastFire;
public:

        unsigned int id;

        virtual void spawn(const EnemyWorldInformation& info);
        virtual void update(EnemyList& list, const EnemyWorldInformation& info);
        virtual void die(EnemyList& list, const EnemyWorldInformation& info);

        virtual ~EnemyAI();
};
