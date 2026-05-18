#include "core/Platform.hpp"

Platform::Platform(sf::Vector2f position, sf::Vector2f size, PlatformType type)
    : mType(type)
{
    mShape.setPosition(position);
    mShape.setSize(size);

    // Визуально разделяем типы платформ цветом
    if (mType == PlatformType::Solid) {
        mShape.setFillColor(sf::Color(70, 150, 70));   // Темно-зеленый для стен и пола
    }
    else {
        mShape.setFillColor(sf::Color(240, 200, 80));  // Желто-песочный для полуплатформ
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