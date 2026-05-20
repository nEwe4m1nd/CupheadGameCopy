#include "entities/EnemyProjectile.hpp"
#include <cmath>

EnemyProjectile::EnemyProjectile(sf::Vector2f position, sf::Vector2f direction, float speed, float damage, bool isParryable)
    : mDirection(direction)
    , mSpeed(speed)
    , mIsActive(true)
    , mDamage(damage)
    , mIsParryable(isParryable)
{
    mShape.setSize({ 16.f, 16.f });
    if (mIsParryable) {
        mShape.setFillColor(sf::Color(255, 105, 180));
    }
    else {
        mShape.setFillColor(sf::Color::Red);
    }
    mShape.setPosition(position);
}

void EnemyProjectile::update(sf::Time deltaTime) {
    mShape.move(mDirection * mSpeed * deltaTime.asSeconds());

    sf::Vector2f pos = mShape.getPosition();
    if (pos.x > 5000.f || pos.x < -1000.f || pos.y > 2000.f || pos.y < -1000.f) {
        mIsActive = false;
    }
}

void EnemyProjectile::draw(sf::RenderTarget& target) const {
    if (mIsActive) target.draw(mShape);
}

void EnemyProjectile::destroy() {
    mIsActive = false;
}

bool EnemyProjectile::isActive() const {
    return mIsActive;
}

bool EnemyProjectile::isParryable() const {
    return mIsParryable;
}

float EnemyProjectile::getDamage() const {
    return mDamage;
}

sf::FloatRect EnemyProjectile::getBounds() const {
    return mShape.getGlobalBounds();
}
