#pragma once
#include <cstdlib>

enum class AttackID {
    None,
    Gatling,    // Семена с неба
    Lunge,      // Удар головой (High/Low)
    Acorns,     // Желуди столбиком
    Boomerang,  // Бумеранг
    Vines       // Лозы с миньонами
};

class SunflowerBrain {
private:
    float mDecisionTimer = 0.0f;
    float mCooldownDuration = 2.0f;

public:
    SunflowerBrain() = default;

    AttackID chooseNextAttack(float dt, float distanceToPlayer, float bossHpPercentage) {
        mDecisionTimer -= dt;
        if (mDecisionTimer > 0.0f) return AttackID::None;

        mDecisionTimer = mCooldownDuration;

        // Выбираем случайную атаку (от 1 до 5)
        int roll = rand() % 100;

        if (roll < 20) return AttackID::Acorns;
        else if (roll < 40) return AttackID::Gatling;
        else if (roll < 60) return AttackID::Boomerang;
        //else if (roll < 80) return AttackID::Vines;
        else return AttackID::Lunge;
    }
};