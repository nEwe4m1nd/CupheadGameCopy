#include "entities/Bullet.hpp"

Bullet::Bullet(sf::Vector2f position, sf::Vector2f velocity)
    : mVelocity(velocity), mIsActive(true)
{
    mShape.setSize({ 10.f, 5.f });
    mShape.setFillColor(sf::Color::Yellow);
    mShape.setPosition(position);
}

void Bullet::update(sf::Time deltaTime) {
    mShape.move(mVelocity * deltaTime.asSeconds());

    // Деактивируем, если улетела далеко (например, за 2000 пикселей)
    if (mShape.getPosition().x > 2000.f || mShape.getPosition().x < -500.f) {
        mIsActive = false;
    }
}

void Bullet::draw(sf::RenderTarget& target) const {
    if (mIsActive) target.draw(mShape);
}