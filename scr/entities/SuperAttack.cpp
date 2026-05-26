#include "entities/SuperAttack.hpp"

SuperAttack::SuperAttack(sf::Vector2f position, sf::Vector2f direction, float damage)
    : mDirection(direction), mSpeed(1200.f), mDamage(damage), mIsActive(true)
{
    mShape.setSize({ 40.f, 40.f });
    mShape.setFillColor(sf::Color::Magenta);
    mShape.setPosition(position);

    float angle = std::atan2(mDirection.y, mDirection.x) * 180.f / 3.14159265f;
    mShape.setRotation(sf::degrees(angle));
}

void SuperAttack::update(sf::Time deltaTime) {
    mShape.move(mDirection * mSpeed * deltaTime.asSeconds());

    if (mShape.getPosition().x > 3000.f || mShape.getPosition().x < -1000.f ||
        mShape.getPosition().y > 2000.f || mShape.getPosition().y < -1000.f) {
        mIsActive = false;
    }
}

void SuperAttack::draw(sf::RenderTarget & target) const {
    if (mIsActive) target.draw(mShape);
}
