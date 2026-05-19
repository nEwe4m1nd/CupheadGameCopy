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

    // движение
    sf::Vector2f moveInput(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) moveInput.x -= mMovementSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) moveInput.x += mMovementSpeed;

    // прыжок
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        if (mIsGrounded || mCanGhostJump) {
            mVelocityY = JUMP_FORCE;
            mIsGrounded = false;
            mCanGhostJump = false;
        }
    }

    if (!mIsGrounded) {
        mVelocityY += GRAVITY * dt;
    }

    // расчет направления взгляда
    sf::Vector2f aimDir(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    aimDir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  aimDir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  aimDir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) aimDir.x += 1.f;

    sf::Vector2f moveDir(0.f, 0.f);
    if (moveInput.x > 0.f) moveDir.x = 1.f;
    if (moveInput.x < 0.f) moveDir.x = -1.f;

    if (aimDir == sf::Vector2f(0.f, 0.f)) aimDir = moveDir;

    if (aimDir != sf::Vector2f(0.f, 0.f)) {
        float length = std::sqrt(aimDir.x * aimDir.x + aimDir.y * aimDir.y);
        mLastLookDirection = aimDir / length;
    }

    // коллизия X
    mPosition.x += moveInput.x * dt;
    mSprite.setPosition(mPosition);

    if (mPlatforms != nullptr) {
        for (const auto& platform : *mPlatforms) {
            if (platform.getType() == PlatformType::OneWay) continue;

            auto intersection = mSprite.getGlobalBounds().findIntersection(platform.getBounds());
            if (intersection.has_value()) {
                if (mSprite.getGlobalBounds().position.x < platform.getBounds().position.x) mPosition.x -= intersection->size.x;
                else mPosition.x += intersection->size.x;
                mSprite.setPosition(mPosition);
            }
        }
    }

    // стрельба
    handleShooting(deltaTime);

    // коллизия Y
    mPosition.y += mVelocityY * dt;
    mSprite.setPosition(mPosition);
    bool touchedGroundThisFrame = false;

    if (mPlatforms != nullptr) {
        for (const auto& platform : *mPlatforms) {
            auto intersection = mSprite.getGlobalBounds().findIntersection(platform.getBounds());
            if (intersection.has_value()) {

                if (platform.getType() == PlatformType::OneWay) {
                    float playerBottom = mSprite.getGlobalBounds().position.y + mSprite.getGlobalBounds().size.y;
                    float platformTop = platform.getBounds().position.y;

                    if (mVelocityY >= 0.f && (playerBottom - intersection->size.y <= platformTop + 5.f)) {
                        // спрыгивание
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                            mVelocityY = 100.f;
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

void Player::handleShooting(sf::Time deltaTime) {
    mShootTimer += deltaTime;

    // обычная атака
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F) && mShootTimer >= SHOOT_COOLDOWN) {
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

        // Проверка столкновения пули с платформами
        if (mPlatforms != nullptr) {
            for (const auto& platform : *mPlatforms) {
                // Если пуля пересекается с платформой — уничтожаем её
                if (bullet.getBounds().findIntersection(platform.getBounds()).has_value()) {
                    bullet.destroy(); // Метод destroy() добавим в класс Bullet ниже
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
