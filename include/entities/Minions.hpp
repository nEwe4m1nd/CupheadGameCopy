#pragma once
#include "entities/Enemy.hpp"
#include "entities/EnemyProjectile.hpp"
#include <vector>

class HomingChomper : public Enemy {
private:
    float mSpeed;

public:
    HomingChomper(sf::Vector2f position);
    void update(sf::Time deltaTime) override; // Обязательный метод базового Entity
    void update(sf::Time deltaTime, sf::Vector2f playerPos) override;
    void draw(sf::RenderTarget& target) const override;
};


class FloorChomper : public Enemy {
private:
    float mSpeed;
    float mMoveDirection;

public:
    FloorChomper(sf::Vector2f position, float startDirection);
    void update(sf::Time deltaTime) override; // Обязательный метод базового Entity
    void update(sf::Time deltaTime, sf::Vector2f playerPos) override;
    void draw(sf::RenderTarget& target) const override;
};


class FlyingChomper : public Enemy {
private:
    float mHoverTimer;
    sf::Vector2f mBasePosition;

    std::vector<EnemyProjectile> mProjectiles;
    sf::Time mShootTimer;
    sf::Time mShootCooldown;

public:
    FlyingChomper(sf::Vector2f position);
    void update(sf::Time deltaTime) override; // Обязательный метод базового Entity
    void update(sf::Time deltaTime, sf::Vector2f playerPos) override;
    void draw(sf::RenderTarget& target) const override;

    std::vector<EnemyProjectile>& getProjectiles();
};