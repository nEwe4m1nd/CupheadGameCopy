#pragma once
#include "include.hpp"

enum class PlatformType {
    Solid,  // стена
    OneWay, // полустена
    Death  // шипы
};

class Platform {
private:
    sf::RectangleShape mShape;
    PlatformType mType;

public:
    Platform(sf::Vector2f position, sf::Vector2f size, PlatformType type);

    sf::FloatRect getBounds() const;
    PlatformType getType() const;
    void draw(sf::RenderTarget& target) const;
};