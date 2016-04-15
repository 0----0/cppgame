#include "EnemyAI.hpp"


class TriShotEnemyAI : public EnemyAI {
public:
        virtual void update(EnemyList& list, const EnemyWorldInformation& info) override;

        virtual ~TriShotEnemyAI();
};
