#include "core/Player.hpp"

Player::Player()
    : mMovementSpeed(300.f)
{
    sf::Image defaultImage({ 50u, 80u }, sf::Color::Cyan);

    if (mTexture.loadFromImage(defaultImage)) {

        mSprite.setTexture(mTexture, true);
    }

    setPosition({ 200.f, 100.f });
}

void Player::update(sf::Time deltaTime) {
    sf::Vector2f movement(0.f, 0.f);
    float dt = deltaTime.asSeconds();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        movement.x -= mMovementSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        movement.x += mMovementSpeed;
    }

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) && mIsGrounded) {
        mVelocityY = JUMP_FORCE;
        mIsGrounded = false;
    }

    if (!mIsGrounded) {
        mVelocityY += GRAVITY * dt;
    }

    movement.y = mVelocityY;
    mPosition += movement * dt;

    if (mPosition.y >= FLOOR_LEVEL) {
        mPosition.y = FLOOR_LEVEL;
        mVelocityY = 0.f;
        mIsGrounded = true;
    }

    mSprite.setPosition(mPosition);
}

void Player::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
}