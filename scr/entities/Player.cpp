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

void Player::update(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();

    // 1. Возвращаем неуязвимость
    if (mIsInvincible) {
        mInvincibilityTimer += deltaTime;
        if (mInvincibilityTimer >= sf::seconds(1.5f)) { // 1.5 секунды неуязвимости
            mIsInvincible = false;
            mSprite.setColor(sf::Color::White);
        }
        else {
            // Эффект мигания при получении урона
            if (static_cast<int>(mInvincibilityTimer.asMilliseconds() / 100) % 2 == 0) {
                mSprite.setColor(sf::Color(255, 100, 100, 150));
            }
            else {
                mSprite.setColor(sf::Color::White);
            }
        }
    }

    if (mLastLookDirection == sf::Vector2f(0.f, 0.f)) {
        mLastLookDirection = { 1.f, 0.f };
    }

    // ghost jump
    if (mIsGrounded) {
        mGhostJumpTimer = sf::Time::Zero;
        mCanGhostJump = true;
        if (!mIsDashing) mCanDash = true;
    }
    else {
        mGhostJumpTimer += deltaTime;
        if (mGhostJumpTimer > GhostJump_DURATION) {
            mCanGhostJump = false;
        }
    }

    // move input & стрельба
    bool tabPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Tab);
    if (tabPressed && !mTabPressedLastFrame) {
        if (mCurrentWeapon == WeaponType::Peashooter) mCurrentWeapon = WeaponType::Spread;
        else if (mCurrentWeapon == WeaponType::Spread) mCurrentWeapon = WeaponType::Chaser;
        else mCurrentWeapon = WeaponType::Peashooter;
    }
    mTabPressedLastFrame = tabPressed;

    bool isLockPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C);
    sf::Vector2f moveInput(0.f, 0.f);

    bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
    bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
    bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);

    if (leftPressed) {
        if (!isLockPressed && !mIsDashing) moveInput.x -= mMovementSpeed;
        mLastLookDirection = { -1.f, 0.f };
    }
    if (rightPressed) {
        if (!isLockPressed && !mIsDashing) moveInput.x += mMovementSpeed;
        mLastLookDirection = { 1.f, 0.f };
    }

    if (upPressed) {
        if (leftPressed) mLastLookDirection = { -0.7071f, -0.7071f };
        else if (rightPressed) mLastLookDirection = { 0.7071f, -0.7071f };
        else mLastLookDirection = { 0.f, -1.f };
    }
    else if (downPressed) {
        if (!mIsGrounded || isLockPressed) {
            if (leftPressed) mLastLookDirection = { -0.7071f, 0.7071f };
            else if (rightPressed) mLastLookDirection = { 0.7071f, 0.7071f };
            else mLastLookDirection = { 0.f, 1.f };
        }
    }

    // деш
    bool shiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift);
    if (shiftPressed && !mDashPressedLastFrame && mCanDash && !mIsDashing) {
        mIsDashing = true;
        mCanDash = false;
        mDashTimer = sf::Time::Zero;
        mDashDirection = mLastLookDirection.x != 0.f ? (mLastLookDirection.x > 0.f ? 1.f : -1.f) : 1.f;
    }
    mDashPressedLastFrame = shiftPressed;

    if (mIsDashing) {
        mDashTimer += deltaTime;
        if (mDashTimer >= DASH_DURATION) mIsDashing = false;
        else {
            moveInput.x = mDashDirection * mMovementSpeed * 2.f;
            mVelocityY = 0.f;
        }
    }

    // прыжок
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
        if (mIsGrounded || mCanGhostJump) {
            mVelocityY = JUMP_FORCE;
            mIsGrounded = false;
            mCanGhostJump = false;
            mIsDashing = false;
        }
    }

    if (!mIsGrounded && !mIsDashing) {
        mVelocityY += GRAVITY * dt;
    }

    // Если стоим на платформе, двигаемся вместе с ней
    if (mIsGrounded) {
        for (const auto& platform : *mPlatforms) {
            // Проверяем, стоит ли игрок на этой конкретной платформе
            if (getBounds().findIntersection(platform.getBounds())) {
                // Добавляем вектор движения платформы к позиции игрока
                mPosition += platform.getVelocity() * deltaTime.asSeconds();
                break;
            }
        }
    }

    // коллизия X (Исправлено отскакивание)
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
                    mIsDashing = false;
                    mSprite.setPosition(mPosition);
                    continue;
                }
                if (platform.getType() == PlatformType::OneWay) continue;

                // Проверяем, что это стена, а не пол
                if (intersection->size.x < intersection->size.y) {
                    if (mPosition.x < platform.getBounds().position.x) mPosition.x -= intersection->size.x;
                    else mPosition.x += intersection->size.x;
                    mSprite.setPosition(mPosition);
                }
            }
        }
    }

    handleShooting(deltaTime);

    // коллизия Y (Исправлено трение и стояние на платформах)
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
                    mIsDashing = false;
                    mSprite.setPosition(mPosition);
                    break;
                }

                if (platform.getType() == PlatformType::OneWay) {
                    float playerBottom = playerBounds.position.y + playerBounds.size.y;
                    float platformTop = platformBounds.position.y;
                    if (mVelocityY >= 0.f && (playerBottom - intersection->size.y <= platformTop + 8.f)) {
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
                            mVelocityY = 200.f;
                            mIsGrounded = false;
                            continue;
                        }
                        mPosition.y -= intersection->size.y;
                        mVelocityY = 0.f;
                        touchedGroundThisFrame = true;
                        mSprite.setPosition(mPosition);
                        mPosition.x += platform.getVelocity().x * dt;
                    }
                    continue;
                }

                // Обработка твердого пола
                if (mVelocityY >= 0.f) {
                    mPosition.y -= intersection->size.y;
                    mVelocityY = 0.f;
                    touchedGroundThisFrame = true;
                    mPosition.x += platform.getVelocity().x * dt;
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

void Player::handleShooting(sf::Time deltaTime) {
    mShootTimer += deltaTime;

    // обычная атака
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


    // супер атака 
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