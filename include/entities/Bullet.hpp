#pragma once
#include "include.hpp"

class Bullet {
private:
    sf::RectangleShape mShape;
    sf::Vector2f mVelocity;
    bool mIsActive;

public:
    Bullet(sf::Vector2f position, sf::Vector2f velocity);
    void update(sf::Time deltaTime);
    void draw(sf::RenderTarget& target) const;

    bool isActive() const { return mIsActive; }
    sf::FloatRect getBounds() const { return mShape.getGlobalBounds(); }
};
