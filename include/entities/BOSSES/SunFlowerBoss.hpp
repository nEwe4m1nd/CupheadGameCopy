#pragma once
#include "entities/Enemy.hpp"
#include "entities/Minions.hpp"
#include "entities/BOSSES/SunflowerStates.hpp" 
#include "entities/BOSSES/SunflowerBrain.hpp"
#include <vector>
#include <memory>

class SunflowerBoss : public Enemy {
private:
    BossContext mContext;
    SunflowerBrain mBrain;
    std::unique_ptr<BossState> mCurrentState;

    sf::Vector2f mBasePosition;
    std::vector<EnemyProjectile> mProjectiles;
    sf::Vector2f mLastPlayerPos; // Для прицеливания атак
    std::vector<std::unique_ptr<Enemy>> mMinions;

public:
    SunflowerBoss(sf::Vector2f position);

    void update(sf::Time deltaTime) override;
    void update(sf::Time deltaTime, sf::Vector2f playerPos) override;
    void draw(sf::RenderTarget& target) const override;

    void changeState(std::unique_ptr<BossState> newState);

    sf::Vector2f getBasePosition() const { return mBasePosition; }
    sf::Vector2f getLastPlayerPos() const { return mLastPlayerPos; }
    std::vector<EnemyProjectile>& getProjectiles() { return mProjectiles; }
    std::vector<std::unique_ptr<Enemy>>& getMinions() { return mMinions; }
};