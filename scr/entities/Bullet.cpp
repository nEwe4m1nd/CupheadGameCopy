#include "entities/Bullet.hpp"

Bullet::Bullet(sf::Vector2f position, sf::Vector2f direction, float speed)
    : mDirection(direction), mSpeed(speed), mIsActive(true)
{
    mShape.setSize({ 12.f, 6.f });
    mShape.setFillColor(sf::Color::Yellow);
    mShape.setPosition(position);

    float angle = std::atan2(mDirection.y, mDirection.x) * 180.f / 3.14159265f;
    mShape.setRotation(sf::degrees(angle));
}

void Bullet::update(sf::Time deltaTime) {
    mShape.move(mDirection * mSpeed * deltaTime.asSeconds());

    if (mShape.getPosition().x > 3000.f || mShape.getPosition().x < -1000.f ||
        mShape.getPosition().y > 2000.f || mShape.getPosition().y < -1000.f) {
        mIsActive = false;
    }
}

void Bullet::draw(sf::RenderTarget& target) const {
    if (mIsActive) target.draw(mShape);
}