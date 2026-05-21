#pragma once
#include "core/Entity.hpp"
#include "core/Platform.hpp"
#include "entities/Bullet.hpp"
#include "entities/SuperAttack.hpp"
#include <vector>

enum class WeaponType { Peashooter, Spread, Chaser };
enum class SuperType { EnergyBeam, Invincibility };

class Player : public Entity {
private:
    float mMovementSpeed;
    float mVelocityY;
    bool mIsGrounded;

    const float GRAVITY = 1980.f;
    const float JUMP_FORCE = -750.f;

    sf::Time mGhostJumpTimer;
    const sf::Time GhostJump_DURATION = sf::seconds(0.07f);
    bool mCanGhostJump;

    const std::vector<Platform>* mPlatforms;

    std::vector<Bullet> mBullets;
    std::vector<SuperAttack> mSuperAttacks;
    sf::Time mShootTimer;
    const sf::Time SHOOT_COOLDOWN = sf::seconds(0.15f);

    sf::Vector2f mLastLookDirection;

    WeaponType mCurrentWeapon;
    SuperType mCurrentSuper;
    float mSuperMeter;

    bool mTabPressedLastFrame;
    bool mDashPressedLastFrame;
    bool mIsDashing;
    sf::Time mDashTimer;
    const sf::Time DASH_DURATION = sf::seconds(0.15f);
    float mDashDirection;
    bool mCanDash;

    // --- НОВОЕ: Здоровье и неуязвимость ---
    int mHp = 3;
    bool mIsInvincible = false;
    sf::Time mInvincibilityTimer = sf::Time::Zero;

public:
    Player();

    void update(sf::Time deltaTime) override;
    void draw(sf::RenderTarget& target) const override;

    void setPlatforms(const std::vector<Platform>& platforms);
    void setWeapon(WeaponType type) { mCurrentWeapon = type; }
    void setSuper(SuperType type) { mCurrentSuper = type; }

    // --- НОВОЕ: Геттеры для Game.cpp и получение урона ---
    std::vector<Bullet>& getBullets() { return mBullets; }
    std::vector<SuperAttack>& getSuperAttacks() { return mSuperAttacks; }
    sf::FloatRect getBounds() const { return mSprite.getGlobalBounds(); }

    void takeDamage(int amount) {
        if (!mIsInvincible) {
            mHp -= amount;
            mIsInvincible = true;
            mInvincibilityTimer = sf::Time::Zero;
        }
    }

private:
    void handleShooting(sf::Time deltaTime);
};