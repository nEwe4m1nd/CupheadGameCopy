#include "entities/Player.hpp"

Player::Player()
    : mMovementSpeed(400.f)
    , mVelocityY(0.f)
    , mIsGrounded(false)
    , mPlatforms(nullptr)
    , mCanGhostJump(false)
    , mLastLookDirection(1.f, 0.f)
    , mCurrentWeapon(WeaponType::Peashooter)
    , mCurrentSuper(SuperType::EnergyBeam)
    , mSuperMeter(0.f)
    , mTabPressedLastFrame(false)
    , mDashPressedLastFrame(false)
    , mIsDashing(false)
    , mDashTimer(sf::Time::Zero)
    , mDashDirection(1.f)
    , mCanDash(true)
    , mHp(3)
    , mMaxHp(3)
    , mIsInvincible(false)
    , mInvincibilityTimer(sf::Time::Zero)
{
    sf::Image defaultImage({ 50u, 80u }, sf::Color::Cyan);
    if (mTexture.loadFromImage(defaultImage)) {
        mSprite.setTexture(mTexture, true);
    }
    setPosition({ 200.f, 100.f });
}

void Player::setPlatforms(const std::vector<Platform>& platforms) {
    mPlatforms = &platforms;
}

void Player::takeDamage(int amount) {
    if (mIsInvincible) return;

    mHp -= amount;
    if (mHp < 0) mHp = 0;

    // включаем неу€звимость после удара
    mIsInvincible = true;
    mInvincibilityTimer = sf::Time::Zero;

    // визуал
    mSprite.setColor(sf::Color(255, 100, 100, 180));
}

void Player::update(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();

    if (mIsInvincible) {
        mInvincibilityTimer += deltaTime;
        if (mInvincibilityTimer >= INVINCIBILITY_DURATION) {
            mIsInvincible = false;
            mSprite.setColor(sf::Color::White);
        }
    }

    if (mLastLookDirection == sf::Vector2f(0.f, 0.f)) {
        mLastLookDirection = { 1.f, 0.f };
    }

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

    bool isLockInPlace = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C);

    // move input
    sf::Vector2f moveInput(0.f, 0.f);
    bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
    bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
    bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);

    if (leftPressed) {
        mLastLookDirection = { -1.f, 0.f };
        if (!isLockInPlace) moveInput.x -= mMovementSpeed;
    }
    if (rightPressed) {
        mLastLookDirection = { 1.f, 0.f };
        if (!isLockInPlace) moveInput.x += mMovementSpeed;
    }

    if (upPressed) {
        if (leftPressed) mLastLookDirection = { -0.7071f, -0.7071f };
        else if (rightPressed) mLastLookDirection = { 0.7071f, -0.7071f };
        else mLastLookDirection = { 0.f, -1.f };
    }
    else if (downPressed) {
        if (!mIsGrounded || isLockInPlace) {
            if (leftPressed) mLastLookDirection = { -0.7071f, 0.7071f };
            else if (rightPressed) mLastLookDirection = { 0.7071f, 0.7071f };
            else mLastLookDirection = { 0.f, 1.f };
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
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

    handleShooting(deltaTime);

    // коллизи€ X
    mPosition.x += moveInput.x * dt;
    mSprite.setPosition(mPosition);

    if (mPlatforms != nullptr) {
        for (const auto& platform : *mPlatforms) {
            auto intersection = mSprite.getGlobalBounds().findIntersection(platform.getBounds());

            if (intersection.has_value()) {
                if (platform.getType() == PlatformType::Death) {
                    takeDamage(1);
                    mPosition = { 200.f, 100.f };
                    mVelocityY = 0.f;
                    mSprite.setPosition(mPosition);
                    continue;
                }

                if (platform.getType() == PlatformType::OneWay) continue;

                if (mPosition.x < platform.getBounds().position.x) {
                    mPosition.x -= intersection->size.x;
                }
                else {
                    mPosition.x += intersection->size.x;
                }
                mSprite.setPosition(mPosition);
            }
        }
    }

    // коллизи€ Y
    mPosition.y += mVelocityY * dt;
    mSprite.setPosition(mPosition);

    bool touchedGroundThisFrame = false;

    if (mPlatforms != nullptr) {
        for (const auto& platform : *mPlatforms) {
            sf::FloatRect playerBounds = mSprite.getGlobalBounds();
            sf::FloatRect platformBounds = platform.getBounds();
            auto intersection = playerBounds.findIntersection(platformBounds);

            if (intersection.has_value()) {
                if (platform.getType() == PlatformType::Death) {
                    takeDamage(1);
                    mPosition = { 200.f, 100.f };
                    mVelocityY = 0.f;
                    mSprite.setPosition(mPosition);
                    break;
                }

                if (platform.getType() == PlatformType::OneWay) {
                    float playerBottom = playerBounds.position.y + playerBounds.size.y;
                    float platformTop = platformBounds.position.y;

                    if (mVelocityY >= 0.f && (playerBottom - intersection->size.y <= platformTop + 8.f)) {
                        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) &&
                            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
                            mVelocityY = 200.f;
                            mIsGrounded = false;
                            continue;
                        }

                        mPosition.y -= intersection->size.y;
                        mVelocityY = 0.f;
                        touchedGroundThisFrame = true;
                        mSprite.setPosition(mPosition);
                    }
                    continue;
                }

                if (intersection->size.x < intersection->size.y) {
                    if (mPosition.x < platformBounds.position.x) {
                        mPosition.x -= intersection->size.x;
                    }
                    else {
                        mPosition.x += intersection->size.x;
                    }
                }
                else {
                    if (mVelocityY > 0.f) {
                        mPosition.y -= intersection->size.y;
                        mVelocityY = 0.f;
                        touchedGroundThisFrame = true;
                    }
                    else if (mVelocityY < 0.f) {
                        mPosition.y += intersection->size.y;
                        mVelocityY = 0.f;
                    }
                }
                mSprite.setPosition(mPosition);
            }
        }
    }

    mIsGrounded = touchedGroundThisFrame;
}

void Player::handleShooting(sf::Time deltaTime) {
    mShootTimer += deltaTime;

    // oбычна€ атака на кнопку X
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X) && mShootTimer >= SHOOT_COOLDOWN) {
        sf::Vector2f spawnPos = mPosition + sf::Vector2f(25.f, 40.f);
        spawnPos += mLastLookDirection * 30.f;

        switch (mCurrentWeapon) {
        case WeaponType::Peashooter:
            mBullets.emplace_back(spawnPos, mLastLookDirection, 900.f);
            break;
        case WeaponType::Spread:
            break;
        case WeaponType::Chaser:
            break;
        }

        if (mSuperMeter < 5.f) {
            mSuperMeter += 0.2f;
        }

        mShootTimer = sf::Time::Zero;
    }

    // супер атака на кнопку V
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V) && mSuperMeter >= 5.f) {
        sf::Vector2f spawnPos = mPosition + sf::Vector2f(25.f, 40.f);
        spawnPos += mLastLookDirection * 40.f;

        switch (mCurrentSuper) {
        case SuperType::EnergyBeam:
            mSuperAttacks.emplace_back(spawnPos, mLastLookDirection);
            break;
        case SuperType::Invincibility:
            break;
        }
        mSuperMeter = 0.f;
    }

    // коллизи€ пуль с картой
    for (auto& bullet : mBullets) {
        bullet.update(deltaTime);

        if (mPlatforms != nullptr) {
            for (const auto& platform : *mPlatforms) {
                if (bullet.getBounds().findIntersection(platform.getBounds()).has_value()) {
                    bullet.destroy();
                    break;
                }
            }
        }
    }

    for (auto& super : mSuperAttacks) {
        super.update(deltaTime);
    }

    mBullets.erase(std::remove_if(mBullets.begin(), mBullets.end(),
        [](const Bullet& b) { return !b.isActive(); }), mBullets.end());

    mSuperAttacks.erase(std::remove_if(mSuperAttacks.begin(), mSuperAttacks.end(),
        [](const SuperAttack& s) { return !s.isActive(); }), mSuperAttacks.end());
}
void Player::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
    for (const auto& bullet : mBullets) {
        bullet.draw(target);
    }
    for (const auto& super : mSuperAttacks) {
        super.draw(target);
    }
}