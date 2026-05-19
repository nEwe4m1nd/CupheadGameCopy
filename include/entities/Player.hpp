#pragma once
#include "core/Entity.hpp"
#include "core/Platform.hpp"
#include "entities/Bullet.hpp"
#include <vector>

enum class WeaponType { Peashooter, Spread, Chaser };
enum class SuperType { EnergyBeam, Invincibility };

class Player : public Entity {
private:
    float mMovementSpeed;
    float mVelocityY;
    bool mIsGrounded;

    // настройки баланса
    const float GRAVITY = 1980.f;
    const float JUMP_FORCE = -750.f;

    // ghost jump
    sf::Time mGhostJumpTimer;
    const sf::Time GhostJump_DURATION = sf::seconds(0.07f);
    bool mCanGhostJump;

    const std::vector<Platform>* mPlatforms;

    //players bullet
    std::vector<Bullet> mBullets;
    sf::Time mShootTimer;
    const sf::Time SHOOT_COOLDOWN = sf::seconds(0.15f);

    sf::Vector2f mLastLookDirection;

    WeaponType mCurrentWeapon;
    SuperType mCurrentSuper;
    float mSuperMeter;

public:
    Player();

public:
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderTarget& target) const override;

public:
    void setPlatforms(const std::vector<Platform>& platforms);
    void setWeapon(WeaponType type) { mCurrentWeapon = type; }
    void setSuper(SuperType type) { mCurrentSuper = type; }

private:
    void handleShooting(sf::Time deltaTime);
};
