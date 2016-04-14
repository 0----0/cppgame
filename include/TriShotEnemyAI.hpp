#include "EnemyAI.hpp"


class TriShotEnemyAI : public EnemyAI {
public:

        // virtual void spawn(TestGame2& game) override;
        virtual void update(EnemyList& list) override;
        // virtual void die(EnemyList& list) override;

        virtual ~TriShotEnemyAI();
};
