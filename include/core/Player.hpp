#pragma once
#include "Entity.hpp"

class Player : public Entity {
private:
    float mMovementSpeed;

    float mVelocityY;
    bool mIsGrounded;

    // настройки баланса
    const float GRAVITY = 1980.f;    // cила притяжения (пиксель/сек^2)
    const float JUMP_FORCE = -750.f; // импульс прыжка
    const float FLOOR_LEVEL = 450.f; // временная хрень

public:
    Player();

    void update(sf::Time deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
};