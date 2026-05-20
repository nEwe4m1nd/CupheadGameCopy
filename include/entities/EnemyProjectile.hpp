#pragma once
#include "include.hpp"

class EnemyProjectile {
private:
    sf::RectangleShape mShape;
    sf::Vector2f mDirection;
    float mSpeed;
    bool mIsActive;
    float mDamage;
    bool mIsParryable;

public:
    EnemyProjectile(sf::Vector2f position, sf::Vector2f direction, float speed, float damage, bool isParryable = false);

    void update(sf::Time deltaTime);
    void draw(sf::RenderTarget& target) const;
    void destroy();

    bool isActive() const;
    bool isParryable() const;
    float getDamage() const;
    sf::FloatRect getBounds() const;
};
