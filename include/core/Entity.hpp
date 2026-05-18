#pragma once
#include"include.hpp"

class Entity {
protected:
    sf::Texture mTexture; 
    sf::Sprite mSprite;
    sf::Vector2f mPosition;

public:
    Entity();
    virtual ~Entity() = default;

public:
    virtual void update(sf::Time deltaTime) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;

// Общие методы для всех сущностей
public:

    void setPosition(sf::Vector2f position);
    sf::Vector2f getPosition() const;
};
