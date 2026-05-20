#pragma once
#include "core/Entity.hpp"

class Enemy : public Entity {
protected:
    float mHp;
    float mDamage;
    bool mIsActive;

public:
    Enemy(sf::Vector2f position, float hp, float damage);
    virtual ~Enemy() = default;

    virtual void update(sf::Time deltaTime, sf::Vector2f playerPos) = 0;
    virtual void takeDamage(float amount);

    bool isActive() const;
    float getDamage() const;
    sf::FloatRect getBounds() const;
    void destroy();
};