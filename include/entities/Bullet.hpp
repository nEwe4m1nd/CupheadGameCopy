#pragma once
#include "include.hpp"

class Bullet {
private:
    sf::RectangleShape mShape;
    sf::Vector2f mDirection;
    float mSpeed;
    bool mIsActive;
    float mDamage;

public:
    Bullet(sf::Vector2f position, sf::Vector2f direction, float speed = 900.f, float damage = 4.f);

public:
    void destroy() { mIsActive = false; }
    void update(sf::Time deltaTime);
    void draw(sf::RenderTarget& target) const;
    float getDamage() const { return mDamage; }

public:
    bool isActive() const { return mIsActive; }
    sf::FloatRect getBounds() const { return mShape.getGlobalBounds(); }
};