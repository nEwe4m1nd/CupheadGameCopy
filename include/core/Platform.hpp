#pragma once
#include "include.hpp"

enum class PlatformType {
    Solid,  // стена
    OneWay // полустена
};

class Platform {
private:
    sf::RectangleShape mShape;
    PlatformType mType; // тип

public:
    Platform(sf::Vector2f position, sf::Vector2f size, PlatformType type = PlatformType::Solid);

public:
    void draw(sf::RenderTarget& target) const;
    sf::FloatRect getBounds() const;
    PlatformType getType() const;  
};