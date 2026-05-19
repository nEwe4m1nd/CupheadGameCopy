#include "entities/SuperAttack.hpp"

SuperAttack::SuperAttack(sf::Vector2f position, sf::Vector2f direction)
    : mDirection(direction), mSpeed(1200.f), mIsActive(true)
{
    // Супер-атака больше по размеру (например, огромный энергетический шар)
    mShape.setSize({ 40.f, 40.f });
    mShape.setFillColor(sf::Color::Magenta); // Фиолетовый/розовый цвет для супера
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
