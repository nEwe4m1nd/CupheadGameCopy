#include "core/Platform.hpp"
#include <cmath>

Platform::Platform(sf::Vector2f position, sf::Vector2f size, PlatformType type, sf::Vector2f moveOffset, float speed)
    : mType(type)
    , mStartPosition(position)
    , mEndPosition(position + moveOffset)
    , mSpeed(speed)
    , mMoveProgress(0.f)
    , mDirection(1)
{
    mShape.setPosition(position);
    mShape.setSize(size);

    if (mType == PlatformType::Solid)   mShape.setFillColor(sf::Color(100, 100, 100));
    if (mType == PlatformType::OneWay)  mShape.setFillColor(sf::Color(0, 255, 0));
    if (mType == PlatformType::Death)   mShape.setFillColor(sf::Color(255, 0, 0));

    if (mSpeed > 0.f && (moveOffset.x != 0.f || moveOffset.y != 0.f)) {
        mIsMoving = true;
        if (mType == PlatformType::Solid) mShape.setFillColor(sf::Color(130, 130, 150));
    }
    else {
        mIsMoving = false;
    }
}

void Platform::update(sf::Time deltaTime) {
    if (!mIsMoving) return;

    sf::Vector2f oldPos = mShape.getPosition();

    float distance = std::sqrt(std::pow(mEndPosition.x - mStartPosition.x, 2) + std::pow(mEndPosition.y - mStartPosition.y, 2));
    if (distance == 0.f) return;

    float timeToTravel = distance / mSpeed;
    mMoveProgress += (deltaTime.asSeconds() / timeToTravel) * mDirection;

    if (mMoveProgress >= 1.f) {
        mMoveProgress = 1.f;
        mDirection = -1;
    }
    else if (mMoveProgress <= 0.f) {
        mMoveProgress = 0.f;
        mDirection = 1;
    }

    sf::Vector2f currentPos = mStartPosition + (mEndPosition - mStartPosition) * mMoveProgress;
    mShape.setPosition(currentPos);

    mVelocity = (currentPos - oldPos) / deltaTime.asSeconds();
}

sf::Vector2f Platform::getVelocity() const {
    return mVelocity;
}

sf::FloatRect Platform::getBounds() const {
    return mShape.getGlobalBounds();
}

PlatformType Platform::getType() const {
    return mType;
}

void Platform::draw(sf::RenderTarget& target) const {
    target.draw(mShape);
}