#include "core/Platform.hpp"

Platform::Platform(sf::Vector2f position, sf::Vector2f size, PlatformType type)
    : mType(type)
{
    mShape.setPosition(position);
    mShape.setSize(size);

    if (mType == PlatformType::Solid) {
        mShape.setFillColor(sf::Color(70, 150, 70)); // темно-зелёные стены
    }
    else {
        mShape.setFillColor(sf::Color(240, 200, 80)); // светло-зеленые полу-стены
    }
}

void Platform::draw(sf::RenderTarget& target) const {
    target.draw(mShape);
}

sf::FloatRect Platform::getBounds() const {
    return mShape.getGlobalBounds();
}

PlatformType Platform::getType() const {
    return mType;
}