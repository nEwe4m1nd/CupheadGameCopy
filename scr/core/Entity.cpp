#include"core/Entity.hpp"

Entity::Entity()
    : mTexture()
    , mSprite(mTexture)
    , mPosition(0.f, 0.f)
{
}

void Entity::setPosition(sf::Vector2f position) {
    mPosition = position;
    mSprite.setPosition(mPosition);
}

sf::Vector2f Entity::getPosition() const {
    return mPosition;
}
