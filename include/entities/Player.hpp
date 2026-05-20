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
    //движение
    float mMovementSpeed;
    float mVelocityY;
    bool mIsGrounded;

    //баланс
    const float GRAVITY = 1980.f;
    const float JUMP_FORCE = -750.f;

    // ghost jump
    sf::Time mGhostJumpTimer;
    const sf::Time GhostJump_DURATION = sf::seconds(0.07f);
    bool mCanGhostJump;

    //контейнер платформ
    const std::vector<Platform>* mPlatforms;

    //players bullet & super_attack
    std::vector<Bullet> mBullets;
    std::vector<SuperAttack> mSuperAttacks;
    sf::Time mShootTimer;
    const sf::Time SHOOT_COOLDOWN = sf::seconds(0.15f);

    //направление взгляда
    sf::Vector2f mLastLookDirection;
    
    //выбор оружия
    WeaponType mCurrentWeapon;
    SuperType mCurrentSuper;
    float mSuperMeter;

    //деш
    bool mTabPressedLastFrame;
    bool mDashPressedLastFrame;
    bool mIsDashing;
    sf::Time mDashTimer;
    const sf::Time DASH_DURATION = sf::seconds(0.15f);
    float mDashDirection;
    bool mCanDash;

    int mHp;
    int mMaxHp;
    bool mIsInvincible; // неуязвимость после получения урона
    sf::Time mInvincibilityTimer;
    const sf::Time INVINCIBILITY_DURATION = sf::seconds(1.5f);

public:
    Player();

public:
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderTarget& target) const override;

public:
    void setPlatforms(const std::vector<Platform>& platforms);
    void setWeapon(WeaponType type) { mCurrentWeapon = type; }
    void setSuper(SuperType type) { mCurrentSuper = type; }

public:
    void takeDamage(int amount);
    int getHp() const { return mHp; }
    bool isInvincible() const { return mIsInvincible; }

    std::vector<Bullet>& getBullets() { return mBullets; }
    sf::FloatRect getBounds() const { return mSprite.getGlobalBounds(); }
    std::vector<SuperAttack>& getSuperAttacks() { return mSuperAttacks; }

private:
    void handleShooting(sf::Time deltaTime);
};
