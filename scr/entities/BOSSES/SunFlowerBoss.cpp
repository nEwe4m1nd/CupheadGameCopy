#include "entities/BOSSES/SunflowerBoss.hpp"
#include "entities/BOSSES/SunflowerStates.hpp"
#include <cmath>
#include <algorithm>

SunflowerBoss::SunflowerBoss(sf::Vector2f position)
    : Enemy(position, 1500.f, 1.f)
    , mBasePosition(position)
{
    if (!mTexture.loadFromFile("assets/sunflower.png")) {
        sf::Image img({ 200u, 300u }, sf::Color::Yellow);
        (void)mTexture.loadFromImage(img);
    }
    mSprite.setTexture(mTexture, true);

    // 1. Устанавливаем новый масштаб босса
    mSprite.setScale({ 1.7f, 1.7f });

    // 2. Берем ГЛОБАЛЬНЫЕ размеры с учетом масштаба 1.6
    float actualHeight = mSprite.getGlobalBounds().size.y;

    // 3. Ставим босса ровно на ту координату Y, которую передали из файла уровня.
    // Если из testLevel.txt придет Y = 950 (пол), мы вычитаем высоту босса, 
    // чтобы его "ноги" ровно касались земли.
    mPosition.y = position.y - actualHeight;
    mSprite.setPosition(mPosition);
    mBasePosition = mPosition;

    // Инициализируем контекст автомата
    mContext.boss = this;
    mContext.player = nullptr;

    changeState(std::make_unique<SunflowerIdleState>(mContext));
}

void SunflowerBoss::changeState(std::unique_ptr<BossState> newState) {
    if (mCurrentState) {
        mCurrentState->exit();
    }
    mCurrentState = std::move(newState);
    if (mCurrentState) {
        mCurrentState->enter();
    }
}

void SunflowerBoss::update(sf::Time deltaTime) {}

void SunflowerBoss::update(sf::Time deltaTime, sf::Vector2f playerPos) {
    float dt = deltaTime.asSeconds();
    mLastPlayerPos = playerPos; // Сохраняем позицию игрока для стейтов атак

    // Обновляем и чистим снаряды босса
    for (auto& proj : mProjectiles) proj.update(deltaTime);
    mProjectiles.erase(std::remove_if(mProjectiles.begin(), mProjectiles.end(),
        [](const EnemyProjectile& p) { return !p.isActive(); }), mProjectiles.end());

    // Обновляем и чистим миньонов (летунов от атаки лозами)
    for (auto& minion : mMinions) {
        minion->update(deltaTime, playerPos);
    }
    mMinions.erase(std::remove_if(mMinions.begin(), mMinions.end(),
        [](const std::unique_ptr<Enemy>& m) { return !m->isActive(); }), mMinions.end());

    float distance = std::abs(playerPos.x - mPosition.x);

    // Работа машины состояний (FSM)
    if (!mCurrentState || mCurrentState->isFinished()) {
        AttackID nextAttack = mBrain.chooseNextAttack(dt, distance, mHp / 1500.f);

        if (nextAttack == AttackID::Acorns) changeState(std::make_unique<SunflowerAcornsState>(mContext));
        else if (nextAttack == AttackID::Gatling) changeState(std::make_unique<SunflowerGatlingState>(mContext));
        else if (nextAttack == AttackID::Boomerang) changeState(std::make_unique<SunflowerBoomerangState>(mContext));
        else if (nextAttack == AttackID::Vines) changeState(std::make_unique<SunflowerVinesState>(mContext));
        else if (nextAttack == AttackID::Lunge) changeState(std::make_unique<SunflowerLungeState>(mContext));
        else {
            if (!mCurrentState || dynamic_cast<SunflowerIdleState*>(mCurrentState.get()) == nullptr) {
                changeState(std::make_unique<SunflowerIdleState>(mContext));
            }
        }
    }

    if (mCurrentState) {
        mCurrentState->update(dt);
    }
}

void SunflowerBoss::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
    for (const auto& proj : mProjectiles) proj.draw(target);
    for (const auto& minion : mMinions) minion->draw(target);
}