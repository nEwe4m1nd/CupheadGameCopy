#include "entities/BOSSES/SunflowerBoss.hpp"
#include <cmath>

SunflowerBoss::SunflowerBoss(sf::Vector2f position)
    : Enemy(position, 500.f, 1.f) // У босса 500 ХП!
    , mState(SunflowerState::Idle)
    , mBasePosition(position)
{
    if (!mTexture.loadFromFile("assets/sunflower.png")) {
        sf::Image img({ 200u, 300u }, sf::Color::Yellow);
        mTexture.loadFromImage(img);
    }
    mSprite.setTexture(mTexture, true);

    // Узнаем реальную высоту загруженной текстуры/хитбокса
    float actualHeight = mSprite.getLocalBounds().size.y;

    // Корректируем Y: поднимаем босса вверх ровно на его высоту.
    // Теперь переданная координата Y будет означать уровень его ног (пола), а не макушки.
    mPosition.y -= actualHeight/2 - 50;

    mSprite.setPosition(mPosition);
    mBasePosition = mPosition; // Обязательно обновляем базовую позицию для анимации покачивания!
}

void SunflowerBoss::update(sf::Time deltaTime) {}

void SunflowerBoss::update(sf::Time deltaTime, sf::Vector2f playerPos) {
    mStateTimer += deltaTime;

    // Простая машина состояний для босса
    if (mState == SunflowerState::Idle) {
        // Качается вверх-вниз
        mPosition.y = mBasePosition.y + std::sin(mStateTimer.asSeconds() * 2.f) * 20.f;
        mSprite.setPosition(mPosition);

        if (mStateTimer.asSeconds() > 3.f) {
            mState = (rand() % 2 == 0) ? SunflowerState::Gatling : SunflowerState::Lunge;
            mStateTimer = sf::Time::Zero;
        }
    }
    else if (mState == SunflowerState::Gatling) {
        mShootTimer += deltaTime;
        if (mShootTimer.asSeconds() >= 0.5f) { // Стреляет каждые полсекунды
            // Летит влево, в сторону игрока
            sf::Vector2f dir(-1.f, (rand() % 100 - 50) / 100.f);
            mProjectiles.emplace_back(mPosition + sf::Vector2f(0.f, 100.f), dir, 400.f, 1.f, true);
            mShootTimer = sf::Time::Zero;
        }
        if (mStateTimer.asSeconds() > 4.f) {
            mState = SunflowerState::Idle;
            mStateTimer = sf::Time::Zero;
        }
    }
    else if (mState == SunflowerState::Lunge) {
        // Резкий рывок влево и плавный возврат
        if (mStateTimer.asSeconds() < 1.f) {
            mPosition.x = mBasePosition.x - std::sin(mStateTimer.asSeconds() * 3.14f) * 300.f;
        }
        else {
            mPosition.x = mBasePosition.x;
            mState = SunflowerState::Idle;
            mStateTimer = sf::Time::Zero;
        }
        mSprite.setPosition(mPosition);
    }

    // Обновляем снаряды
    for (auto& proj : mProjectiles) proj.update(deltaTime);
    mProjectiles.erase(std::remove_if(mProjectiles.begin(), mProjectiles.end(),
        [](const EnemyProjectile& p) { return !p.isActive(); }), mProjectiles.end());
}

void SunflowerBoss::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
    for (const auto& proj : mProjectiles) proj.draw(target);
}