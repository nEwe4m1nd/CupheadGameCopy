#pragma once
#include "include.hpp"

enum class PlatformType {
    Solid,  // стена
    OneWay, // полустена
    Death   // шипы
};

class Platform {
private:
    sf::RectangleShape mShape;
    PlatformType mType;
    bool mIsVisible = true;

    bool mIsMoving;
    sf::Vector2f mStartPosition;
    sf::Vector2f mEndPosition;
    float mSpeed;
    float mMoveProgress;
    int mDirection;
    sf::Vector2f mVelocity;

    sf::Texture mTexture;

public:
   Platform(sf::Vector2f position, sf::Vector2f size, PlatformType type,
        sf::Vector2f moveOffset = sf::Vector2f(0.f, 0.f), float speed = 0.f);

public:
    void update(sf::Time deltaTime);
    void draw(sf::RenderTarget& target) const;

public:
    void setVisible(bool visible) { mIsVisible = visible; }
    bool isVisible() const { return mIsVisible; }

public:
    sf::FloatRect getBounds() const;
    PlatformType getType() const;
    sf::Vector2f getVelocity() const;
};