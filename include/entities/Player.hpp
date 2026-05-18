#pragma once
#include "core/Entity.hpp"
#include "core/Platform.hpp"
#include <vector>

class Player : public Entity {
private:
    float mMovementSpeed;

    float mVelocityY;
    bool mIsGrounded;

    // настройки баланса
    const float GRAVITY = 1980.f;    // cила притяжения (пиксель/сек^2)
    const float JUMP_FORCE = -750.f; // импульс прыжка

    const std::vector<Platform>* mPlatforms; // указатель на платформы

public:
    Player();

    void update(sf::Time deltaTime) override;
    void draw(sf::RenderTarget& target) const override;

    void setPlatforms(const std::vector<Platform>& platforms);
};