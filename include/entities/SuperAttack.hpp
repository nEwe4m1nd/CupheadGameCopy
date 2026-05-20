#include "include.hpp"

class SuperAttack {
private:
    sf::RectangleShape mShape;
    sf::Vector2f mDirection;
    float mSpeed;
    bool mIsActive;
    float mDamage;

public:
    SuperAttack(sf::Vector2f position, sf::Vector2f direction, float damage = 35.f);

public:
    void update(sf::Time deltaTime);
    void draw(sf::RenderTarget& target) const;
    float getDamage() const { return mDamage; }

public:
    bool isActive() const { return mIsActive; }
    sf::FloatRect getBounds() const { return mShape.getGlobalBounds(); }
};
