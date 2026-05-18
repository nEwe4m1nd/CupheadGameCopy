#include "entities/Player.hpp"

Player::Player()
    : mMovementSpeed(400.f)
    , mVelocityY(0.f)
    , mIsGrounded(false)
    , mPlatforms(nullptr) // При создании игрок пока не знает про карту
{
    sf::Image defaultImage({ 50u, 80u }, sf::Color::Cyan);
    if (mTexture.loadFromImage(defaultImage)) {
        mSprite.setTexture(mTexture, true);
    }
    setPosition({ 200.f, 100.f });
}

void Player::setPlatforms(const std::vector<Platform>& platforms) {
    mPlatforms = &platforms; // Запоминаем адрес вектора платформ
}

void Player::update(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();

    // ghost jump
    if (mIsGrounded) {
        mGhostJumpTimer = sf::Time::Zero;
        mCanGhostJump = true;
    }
    else {
        mGhostJumpTimer += deltaTime;
        if (mGhostJumpTimer > GhostJump_DURATION) {
            mCanGhostJump = false;
        }
    }

    // move input
    sf::Vector2f moveInput(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        moveInput.x -= mMovementSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        moveInput.x += mMovementSpeed;
    }

    // Прыжок срабатывает, если мы на земле ИЛИ в окне Ghost Jump
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))) {
        if (mIsGrounded || mCanGhostJump) {
            mVelocityY = JUMP_FORCE;
            mIsGrounded = false;
            mCanGhostJump = false;
        }
    }

    // применение гравитации
    if (!mIsGrounded) {
        mVelocityY += GRAVITY * dt;
    }

    //коллизия
    mPosition.x += moveInput.x * dt;
    mSprite.setPosition(mPosition);

    if (mPlatforms != nullptr) {
        for (const auto& platform : *mPlatforms) {
            auto intersection = mSprite.getGlobalBounds().findIntersection(platform.getBounds());
            if (intersection.has_value()) {
                
                if (mSprite.getGlobalBounds().position.x < platform.getBounds().position.x) {
                    mPosition.x -= intersection->size.x;
                }
                else {
                    mPosition.x += intersection->size.x;
                }
                mSprite.setPosition(mPosition);
            }
        }
    }

    mPosition.y += mVelocityY * dt;
    mSprite.setPosition(mPosition);

    bool touchedGroundThisFrame = false;

    if (mPlatforms != nullptr) {
        for (const auto& platform : *mPlatforms) {
            auto intersection = mSprite.getGlobalBounds().findIntersection(platform.getBounds());
            if (intersection.has_value()) {

                if (mVelocityY > 0.f) {
                    
                    mPosition.y -= intersection->size.y;
                    mVelocityY = 0.f;
                    touchedGroundThisFrame = true;
                }
                else if (mVelocityY < 0.f) {
                    
                    mPosition.y += intersection->size.y;
                    mVelocityY = 0.f;
                }
                mSprite.setPosition(mPosition);
            }
        }
    }

    mIsGrounded = touchedGroundThisFrame;
}

void Player::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
}