#include "entities/Minions.hpp"
#include <cmath>

// HomingChomper
HomingChomper::HomingChomper(sf::Vector2f position)
    : Enemy(position, 10.f, 1.f)
    , mSpeed(250.f)
{
    sf::Image img({ 40u, 40u }, sf::Color::Green);
    mTexture.loadFromImage(img);
    mSprite.setTexture(mTexture, true);
}

void HomingChomper::update(sf::Time deltaTime) {}

void HomingChomper::update(sf::Time deltaTime, sf::Vector2f playerPos) {
    sf::Vector2f direction = playerPos - mPosition;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0.f) {
        direction.x /= length;
        direction.y /= length;
    }

    mPosition += direction * mSpeed * deltaTime.asSeconds();
    mSprite.setPosition(mPosition);
}

void HomingChomper::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
}


// FloorChomper
FloorChomper::FloorChomper(sf::Vector2f position, float startDirection)
    : Enemy(position, 15.f, 1.f)
    , mSpeed(150.f)
    , mMoveDirection(startDirection)
{
    sf::Image img({ 50u, 30u }, sf::Color(128, 0, 128));
    mTexture.loadFromImage(img);
    mSprite.setTexture(mTexture, true);
}

void FloorChomper::update(sf::Time deltaTime) {}

void FloorChomper::update(sf::Time deltaTime, sf::Vector2f playerPos) {
    mPosition.x += mMoveDirection * mSpeed * deltaTime.asSeconds();
    mSprite.setPosition(mPosition);
}

void FloorChomper::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
}


// FlyingChomper
FlyingChomper::FlyingChomper(sf::Vector2f position)
    : Enemy(position, 20.f, 1.f)
    , mHoverTimer(0.f)
    , mBasePosition(position)
    , mShootCooldown(sf::seconds(3.f))
{
    sf::Image img({ 45u, 45u }, sf::Color(255, 165, 0));
    mTexture.loadFromImage(img);
    mSprite.setTexture(mTexture, true);
    mShootTimer = sf::seconds(1.5f);
}

void FlyingChomper::update(sf::Time deltaTime) {}

void FlyingChomper::update(sf::Time deltaTime, sf::Vector2f playerPos) {
    mHoverTimer += deltaTime.asSeconds();
    mPosition.y = mBasePosition.y + std::sin(mHoverTimer * 3.f) * 30.f;
    mSprite.setPosition(mPosition);

    mShootTimer += deltaTime;
    if (mShootTimer >= mShootCooldown) {
        sf::Vector2f direction = playerPos - mPosition;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0.f) {
            direction.x /= length;
            direction.y /= length;
        }

        mProjectiles.emplace_back(mPosition + sf::Vector2f(22.f, 22.f), direction, 200.f, 1.f, true);
        mShootTimer = sf::Time::Zero;
    }

    for (auto& proj : mProjectiles) {
        proj.update(deltaTime);
    }

    mProjectiles.erase(std::remove_if(mProjectiles.begin(), mProjectiles.end(),
        [](const EnemyProjectile& p) { return !p.isActive(); }), mProjectiles.end());
}

void FlyingChomper::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
    for (const auto& proj : mProjectiles) {
        proj.draw(target);
    }
}

std::vector<EnemyProjectile>& FlyingChomper::getProjectiles() {
    return mProjectiles;
}