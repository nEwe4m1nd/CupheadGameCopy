#include "include.hpp"

class SuperAttack {
private:
    sf::RectangleShape mShape;
    sf::Vector2f mDirection;
    float mSpeed;
    bool mIsActive;

public:
    SuperAttack(sf::Vector2f position, sf::Vector2f direction);
    void update(sf::Time deltaTime);
    void draw(sf::RenderTarget& target) const;

    bool isActive() const { return mIsActive; }
    sf::FloatRect getBounds() const { return mShape.getGlobalBounds(); }
};
