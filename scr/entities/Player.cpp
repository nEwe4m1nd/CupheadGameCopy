#include "entities/Player.hpp"

Player::Player()
    : mMovementSpeed(400.f)
    , mVelocityY(0.f)
    , mIsGrounded(false)
    , mPlatforms(nullptr) // При создании игрок пока не знает про карту
{
    sf::Image defaultImage({ 50u, 80u }, sf::Color::Cyan);
    if (mTexture.loadFromImage(defaultImage)) {
        mSprite.setTexture(mTexture, true);
    }
    setPosition({ 200.f, 100.f });
}

void Player::setPlatforms(const std::vector<Platform>& platforms) {
    mPlatforms = &platforms; // Запоминаем адрес вектора платформ
}

// Не забудь изменить сигнатуру метода update:
void Player::update(sf::Time deltaTime) {
    sf::Vector2f movement(0.f, 0.f);
    float dt = deltaTime.asSeconds();

    // 1. Бег
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        movement.x -= mMovementSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        movement.x += mMovementSpeed;
    }

    // 2. Прыжок
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) && mIsGrounded) {
        mVelocityY = JUMP_FORCE;
        mIsGrounded = false;
    }

    // 3. Гравитация
    if (!mIsGrounded) {
        mVelocityY += GRAVITY * dt;
    }

    movement.y = mVelocityY;
    mPosition += movement * dt;
    mSprite.setPosition(mPosition);

    // 4. Умные коллизии
    mIsGrounded = false;

    if (mPlatforms != nullptr) {
        for (const auto& platform : *mPlatforms) {
            auto intersection = mSprite.getGlobalBounds().findIntersection(platform.getBounds());

            if (intersection.has_value()) {

                // --- МЕХАНИКА ДЛЯ ПОЛУПЛАТФОРМ (One-Way) ---
                if (platform.getType() == PlatformType::OneWay) {
                    // Персонаж реагирует на нее только если падает (velocity > 0)
                    // и его ноги находятся примерно на уровне верхушки платформы
                    float playerBottom = mSprite.getGlobalBounds().position.y + mSprite.getGlobalBounds().size.y;
                    float platformTop = platform.getBounds().position.y;

                    if (mVelocityY > 0.f && (playerBottom - intersection->size.y <= platformTop + 10.f)) {
                        if (intersection->size.x > intersection->size.y) { // Столкновение строго вертикальное
                            mPosition.y -= intersection->size.y;
                            mVelocityY = 0.f;
                            mIsGrounded = true;
                        }
                    }
                    mSprite.setPosition(mPosition);
                    continue; // Переходим к следующей платформе
                }

                // --- МЕХАНИКА ДЛЯ ТВЕРДЫХ ПЛАТФОРМ (Solid 4-Way) ---
                if (intersection->size.x > intersection->size.y) {
                    // Вертикальная коллизия (перекрытие по X больше, значит удар снизу или сверху)
                    if (mSprite.getGlobalBounds().position.y < platform.getBounds().position.y) {
                        // Игрок выше платформы -> Приземление
                        mPosition.y -= intersection->size.y; // Выталкиваем вверх
                        mVelocityY = 0.f;
                        mIsGrounded = true;
                    }
                    else {
                        // Игрок ниже платформы -> Ударился головой о потолок
                        mPosition.y += intersection->size.y; // Выталкиваем вниз
                        mVelocityY = 0.f;
                    }
                }
                else {
                    // Горизонтальная коллизия (Врезались в торец / стену)
                    if (mSprite.getGlobalBounds().position.x < platform.getBounds().position.x) {
                        // Игрок левее платформы -> Выталкиваем влево
                        mPosition.x -= intersection->size.x;
                    }
                    else {
                        // Игрок правее платформы -> Выталкиваем вправо
                        mPosition.x += intersection->size.x;
                    }
                }
                mSprite.setPosition(mPosition); // Мгновенно применяем выталкивание
            }
        }
    }
}

void Player::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
}