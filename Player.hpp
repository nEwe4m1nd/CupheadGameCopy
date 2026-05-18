#pragma once
#include "core/Entity.hpp"

class Player : public Entity {
private:
    float mMovementSpeed;

public:
    Player();

    // Переопределяем методы базового класса
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
};