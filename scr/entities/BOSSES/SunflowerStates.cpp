#include "entities/BOSSES/SunflowerStates.hpp"
#include "entities/BOSSES/SunflowerBoss.hpp"
#include "entities/Player.hpp" 
#include <cmath>
#include <algorithm>
#include <random> 
#include <ctime>

// Вспомогательная функция для определения ближайшей линии атаки (Сетка из 3 линий)
float getClosestLaneY(float playerY, float baseY) {
    float topLane = baseY - 150.f; // Верхняя линия
    float middleLane = baseY;         // Средняя линия
    float bottomLane = baseY + 150.f; // Нижняя линия

    float distTop = std::abs(playerY - topLane);
    float distMid = std::abs(playerY - middleLane);
    float distBot = std::abs(playerY - bottomLane);

    if (distTop < distMid && distTop < distBot) return topLane;
    if (distBot < distMid && distBot < distTop) return bottomLane;
    return middleLane; // Если ближе всего к центру
}

// --- IDLE STATE ---
void SunflowerIdleState::enter() {}
void SunflowerIdleState::update(float dt) {
    BossState::update(dt);
    sf::Vector2f pos = ctx.boss->getPosition();
    pos.y = ctx.boss->getBasePosition().y + std::sin(stateTimer * 2.f) * 20.f;
    ctx.boss->setPosition(pos);
    if (stateTimer > 2.0f) isCompleted = true;
}
void SunflowerIdleState::exit() {}

// ==========================================
// 1. АТАКА ЖЕЛУДЯМИ 
// ==========================================
void SunflowerAcornsState::enter() {
    shotsFired = 0;
    fireTimer = 1.5f;
    fireOrder = { 0, 1, 2 };

    std::default_random_engine rng(static_cast<unsigned>(std::time(nullptr)));
    std::shuffle(fireOrder.begin(), fireOrder.end(), rng);
}
void SunflowerAcornsState::update(float dt) {
    BossState::update(dt);
    fireTimer -= dt;

    if (fireTimer <= 0.0f && shotsFired < 3) {
        // Координата X: ровно 30 единиц перед боссом
        float spawnX = ctx.boss->getPosition().x - 30.f;

        // Координата Y: центральная точка босса - 15, + шаг 15 единиц
        // fireOrder хранит числа 0, 1 или 2 в случайном порядке
        float spawnY = 320.f + (fireOrder[shotsFired] * 35.f);

        sf::Vector2f spawnPos(spawnX, spawnY);

        sf::Vector2f targetPos = ctx.boss->getLastPlayerPos();
        sf::Vector2f direction = targetPos - spawnPos;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length != 0) { direction.x /= length; direction.y /= length; }

        ctx.boss->getProjectiles().emplace_back(spawnPos, direction, 550.f, 1.f, false);

        shotsFired++;
        fireTimer = 0.4f;
    }

    if (shotsFired >= 3 && fireTimer <= -1.0f) isCompleted = true;
}
void SunflowerAcornsState::exit() {}

// ==========================================
// 2. ГАТЛИНГ (Периодичность 1с, ровно 12 пуль, скорость / 2)
// ==========================================
void SunflowerGatlingState::enter() {
    shootTimer = 0.0f;
    shotsFired = 0;
}
void SunflowerGatlingState::update(float dt) {
    BossState::update(dt);
    shootTimer += dt;

    // Вылетают строго с периодичностью в 1 секунду по 1 штуке
    if (shootTimer >= 1.0f && shotsFired < 12) {
        // Спавн сверху экрана по случайной координате X
        float randX = static_cast<float>((rand() % 1400) + 100);
        sf::Vector2f spawnPos(randX, -50.f);
        sf::Vector2f direction(0.f, 1.f); // Падают строго вниз

        // Шанс 30% на розовую пулю
        bool isPink = (rand() % 100 < 30);

        // СКОРОСТЬ СНИЖЕНА В 2 РАЗА (было 350.f -> стало 175.f)
        ctx.boss->getProjectiles().emplace_back(spawnPos, direction, 175.f, 1.f, isPink);

        shotsFired++;
        shootTimer = 0.0f; // Сброс таймера до следующей секунды
    }

    // Завершаем атаку, когда все 12 снарядов выпущены
    if (shotsFired >= 12 && shootTimer > 1.0f) {
        isCompleted = true;
    }
}
void SunflowerGatlingState::exit() {}

// ==========================================
// 3. БУМЕРАНГ (По четким линиям атаки)
// ==========================================
void SunflowerBoomerangState::enter() {
    returningPhase = false;

    // Вылетает всегда по фиксированной Средней линии босса
    sf::Vector2f spawnPos = ctx.boss->getPosition();
    spawnPos.y = ctx.boss->getBasePosition().y + 340.f;
    sf::Vector2f direction(-1.f, 0.f);

    ctx.boss->getProjectiles().emplace_back(spawnPos, direction, 600.f, 1.f, false);
}
void SunflowerBoomerangState::update(float dt) {
    BossState::update(dt);

    // Ждем, пока улетит за левый край экрана
    if (stateTimer > 2.5f && !returningPhase) {
        returningPhase = true;

        // РАСЧЕТ: К какой из трех линий атаки ближе всего игрок в момент возврата?
        float playerY = ctx.boss->getLastPlayerPos().y;
        float targetLaneY = getClosestLaneY(playerY, ctx.boss->getBasePosition().y);

        // Бумеранг возвращается строго по этой вычисленной горизонтальной траектории
        sf::Vector2f returnPos(-100.f, targetLaneY + 170);
        sf::Vector2f returnDir(1.f, 0.f);

        ctx.boss->getProjectiles().emplace_back(returnPos, returnDir, 600.f, 1.f, false);
    }

    if (stateTimer > 5.0f) isCompleted = true;
}
void SunflowerBoomerangState::exit() {}

// ==========================================
// 4. ЛОЗЫ (Выросли -> оставили миньона -> уползли)
// ==========================================
void SunflowerVinesState::enter() {
    vinesSpawned = 0;
    vineTimer = 0.0f;
}
void SunflowerVinesState::update(float dt) {
    BossState::update(dt);
    vineTimer += dt;

    // Интервал между лозами 1.5 секунды, максимум 3 лозы
    if (vineTimer >= 1.5f && vinesSpawned < 3) {
        float randX = static_cast<float>((rand() % 1100) + 200);

        // Лоза вылезает из-под земли (например, Y = 900)
        sf::Vector2f groundPos(randX, 900.f);

        // Логика: Выросла, оставила летающего миньона на верхушке и уползла назад.
        // Спавним FlyingChomper, который сам по себе будет летать по карте и атаковать
        ctx.boss->getMinions().push_back(std::make_unique<FlyingChomper>(groundPos));

        // Примечание: Если у тебя есть отдельный класс спрайта/анимации лозы VineObject,
        // ты можешь спавнить его здесь параллельно с миньоном, задав ему время жизни 1 сек.

        vinesSpawned++;
        vineTimer = 0.0f;
    }

    if (vinesSpawned >= 3 && vineTimer > 1.5f) isCompleted = true;
}
void SunflowerVinesState::exit() {}

// ==========================================
// 5. АТАКА ГОЛОВОЙ (По четкой линии, СКОРОСТЬ СНИЖЕНА В 6 РАЗ)
// ==========================================
void SunflowerLungeState::enter() {
    // РАСЧЕТ: К какой из трех линий атаки ближе всего игрок в момент старта?
    float playerY = ctx.boss->getLastPlayerPos().y;
    float targetLaneY = getClosestLaneY(playerY, ctx.boss->getBasePosition().y);

    // Запоминаем точную высоту выбранной линии атаки
    sf::Vector2f pos = ctx.boss->getPosition();
    pos.y = targetLaneY;
    ctx.boss->setPosition(pos);
}
void SunflowerLungeState::update(float dt) {
    BossState::update(dt);
    sf::Vector2f pos = ctx.boss->getPosition();

    // СКОРОСТЬ СНИЖЕНА В 6 РАЗ: 
    // Прежний цикл выпада занимал 0.6 сек. Теперь он занимает 0.6 * 6 = 3.6 секунды!
    float totalDuration = 3.6f;

    if (stateTimer < totalDuration) {
        // Синусоида растянута на 3.6 секунды, движение стало очень плавным и читаемым
        float angle = stateTimer * (3.14159265f / totalDuration);
        pos.x = ctx.boss->getBasePosition().x - std::sin(angle) * 1200.f;
    }
    else {
        // Возвращаемся в исходную точку
        pos.x = ctx.boss->getBasePosition().x;
        pos.y = ctx.boss->getBasePosition().y;
        isCompleted = true;
    }

    ctx.boss->setPosition(pos);
}
void SunflowerLungeState::exit() {}