#pragma once
#include "entities/Enemy.hpp"
#include "entities/EnemyProjectile.hpp"
#include <vector>

enum class SunflowerState { Idle, Gatling, Lunge };

class SunflowerBoss : public Enemy {
private:
    SunflowerState mState;
    sf::Time mStateTimer;
    sf::Vector2f mBasePosition;

    std::vector<EnemyProjectile> mProjectiles;
    sf::Time mShootTimer;

public:
    SunflowerBoss(sf::Vector2f position);
    void update(sf::Time deltaTime) override;
    void update(sf::Time deltaTime, sf::Vector2f playerPos) override;
    void draw(sf::RenderTarget& target) const override;

    std::vector<EnemyProjectile>& getProjectiles() { return mProjectiles; }
};