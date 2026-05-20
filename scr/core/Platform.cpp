#include "core/Platform.hpp"

Platform::Platform(sf::Vector2f position, sf::Vector2f size, PlatformType type)
    : mType(type)
{
    mShape.setPosition(position);
    mShape.setSize(size);

    if (mType == PlatformType::Solid)   mShape.setFillColor(sf::Color(100, 100, 100));
    if (mType == PlatformType::OneWay)  mShape.setFillColor(sf::Color(0, 255, 0));
    if (mType == PlatformType::Death)   mShape.setFillColor(sf::Color(255, 0, 0));
}

sf::FloatRect Platform::getBounds() const {
    return mShape.getGlobalBounds();
}

PlatformType Platform::getType() const {
    return mType;
}

void Platform::draw(sf::RenderTarget& target) const {
    target.draw(mShape);
}