#include "entities/Enemy.hpp"

Enemy::Enemy(sf::Vector2f position, float hp, float damage)
    : mHp(hp)
    , mDamage(damage)
    , mIsActive(true)
{
    setPosition(position);
}

void Enemy::takeDamage(float amount) {
    mHp -= amount;
    if (mHp <= 0.f) {
        mIsActive = false;
    }
}

bool Enemy::isActive() const {
    return mIsActive;
}

float Enemy::getDamage() const {
    return mDamage;
}

sf::FloatRect Enemy::getBounds() const {
    return mSprite.getGlobalBounds();
}

void Enemy::destroy() {
    mIsActive = false;
}