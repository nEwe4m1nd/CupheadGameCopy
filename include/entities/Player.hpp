 #pragma once
#include "core/Entity.hpp"
#include "core/Platform.hpp"
#include "entities/Bullet.hpp"
#include <vector>

class Player : public Entity {
private:
    float mMovementSpeed;
    float mVelocityY;
    bool mIsGrounded;

    // настройки баланса
    const float GRAVITY = 1980.f;    // cила притяжения (пиксель/сек^2)
    const float JUMP_FORCE = -750.f; // импульс прыжка

    // ghost jump
    sf::Time mGhostJumpTimer;
    const sf::Time GhostJump_DURATION = sf::seconds(0.07f); // 70 миллисекунд окна для прыжка
    bool mCanGhostJump;

    const std::vector<Platform>* mPlatforms; // указатель на платформы

    //players bullet
    std::vector<Bullet> mBullets;
    sf::Time mShootTimer;
    const sf::Time SHOOT_COOLDOWN = sf::seconds(0.15f);

public:
    Player();

public:
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderTarget& target) const override;

public:
    void setPlatforms(const std::vector<Platform>& platforms);

private:
    void handleShooting(sf::Time deltaTime);
};
