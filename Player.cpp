#include "core/Player.hpp"

Player::Player()
    : mMovementSpeed(300.f)
{
    sf::Image defaultImage({ 50u, 80u }, sf::Color::Cyan);

    if (mTexture.loadFromImage(defaultImage)) {
        // Второй параметр true принудительно сбрасывает прямоугольник отрисовки 
        // под новый реальный размер загруженной картинки (50x80)
        mSprite.setTexture(mTexture, true);
    }

    setPosition({ 200.f, 400.f }); // Поднимем чуть выше, чтобы точно не промахнуться мимо экрана
}

void Player::update(sf::Time deltaTime) {
    sf::Vector2f movement(0.f, 0.f);

    // Считываем нажатия клавиш
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        movement.x -= mMovementSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        movement.x += mMovementSpeed;
    }

    // Применяем движение с учетом deltaTime (чтобы скорость не зависела от FPS)
    mPosition += movement * deltaTime.asSeconds();
    mSprite.setPosition(mPosition);
}

void Player::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
}