#include "entities/BOSSES/SunflowerStates.hpp"
#include "entities/BOSSES/SunflowerBoss.hpp"
#include "entities/Player.hpp" 
#include <cmath>
#include <algorithm>
#include <random> 
#include <ctime>

float getClosestLaneY(float playerY, float baseY) {
    float topLane = baseY - 450.f; // Верхняя линия
    float middleLane = baseY;         // Средняя линия
    float bottomLane = baseY + 450.f; // Нижняя линия

    float distTop = std::abs(playerY - topLane);
    float distMid = std::abs(playerY - middleLane);
    float distBot = std::abs(playerY - bottomLane);

    if (distTop < distMid && distTop < distBot) return topLane;
    if (distBot < distMid && distBot < distTop) return bottomLane;
    return middleLane;
}


void SunflowerIdleState::enter() {}
void SunflowerIdleState::update(float dt) {
    BossState::update(dt);
    sf::Vector2f pos = ctx.boss->getPosition();
    pos.y = ctx.boss->getBasePosition().y + std::sin(stateTimer * 2.f) * 20.f;
    ctx.boss->setPosition(pos);
    if (stateTimer > 2.0f) isCompleted = true;
}
void SunflowerIdleState::exit() {}

// 1. АТАКА ЖЕЛУДЯМИ 
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
        float spawnX = ctx.boss->getPosition().x - 30.f;

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

// 2. ГАТЛИНГ (Периодичность 1с, ровно 12 пуль, скорость / 2)
void SunflowerGatlingState::enter() {
    shootTimer = 0.0f;
    shotsFired = 0;
}
void SunflowerGatlingState::update(float dt) {
    BossState::update(dt);
    shootTimer += dt;

    if (shootTimer >= 1.0f && shotsFired < 12) {
        float randX = static_cast<float>((rand() % 1400) + 100);
        sf::Vector2f spawnPos(randX, -50.f);
        sf::Vector2f direction(0.f, 1.f); 

        bool isPink = (rand() % 100 < 30);

        ctx.boss->getProjectiles().emplace_back(spawnPos, direction, 175.f, 1.f, isPink);

        shotsFired++;
        shootTimer = 0.0f; 
    }

    if (shotsFired >= 12 && shootTimer > 1.0f) {
        isCompleted = true;
    }
}
void SunflowerGatlingState::exit() {}


// 3. БУМЕРАНГ 
void SunflowerBoomerangState::enter() {
    returningPhase = false;

    sf::Vector2f spawnPos = ctx.boss->getPosition();
    spawnPos.y = ctx.boss->getBasePosition().y + 340.f;
    sf::Vector2f direction(-1.f, 0.f);

    ctx.boss->getProjectiles().emplace_back(spawnPos, direction, 600.f, 1.f, false);
}
void SunflowerBoomerangState::update(float dt) {
    BossState::update(dt);

    if (stateTimer > 2.5f && !returningPhase) {
        returningPhase = true;

        sf::Vector2f returnPos(-150.f, 700.f);
        sf::Vector2f returnDir(1.f, 0.f);

        ctx.boss->getProjectiles().emplace_back(returnPos, returnDir, 600.f, 1.f, false);
    }

    if (stateTimer > 5.0f) isCompleted = true;
}
void SunflowerBoomerangState::exit() {}

// 4. ЛОЗЫ 
void SunflowerVinesState::enter() {
    vinesSpawned = 0;
    vineTimer = 0.0f;
}
void SunflowerVinesState::update(float dt) {
    BossState::update(dt);
    vineTimer += dt;

    if (vineTimer >= 1.5f && vinesSpawned < 3) {
        float randX = static_cast<float>((rand() % 1100) + 200);

        sf::Vector2f groundPos(randX, 900.f);

        ctx.boss->getMinions().push_back(std::make_unique<FlyingChomper>(groundPos));

        vinesSpawned++;
        vineTimer = 0.0f;
    }

    if (vinesSpawned >= 3 && vineTimer > 1.5f) isCompleted = true;
}
void SunflowerVinesState::exit() {}

// 5. АТАКА ГОЛОВОЙ 
void SunflowerLungeState::enter() {
    float playerY = ctx.boss->getLastPlayerPos().y;

    float baseLaneY = ctx.boss->getBasePosition().y;

    float topLane = baseLaneY + 300.f;
    float bottomLane = baseLaneY + 450.f;

    std::vector<float> validLanes;

    if (topLane < 510.f) validLanes.push_back(topLane);
    if (bottomLane < 510.f) validLanes.push_back(bottomLane);

    float targetLaneY = validLanes[0];
    float bestDist = std::abs(playerY - targetLaneY);

    for (float lane : validLanes) {
        float d = std::abs(playerY - lane);
        if (d < bestDist) {
            bestDist = d;
            targetLaneY = lane;
        }
    }

    sf::Vector2f pos = ctx.boss->getPosition();
    pos.y = targetLaneY;
    ctx.boss->setPosition(pos);
}
void SunflowerLungeState::update(float dt) {
    BossState::update(dt);

    sf::Vector2f pos = ctx.boss->getPosition();

    float totalDuration = 3.6f;

    float fixedY = pos.y;

    if (stateTimer < totalDuration) {
        float angle = stateTimer * (3.14159265f / totalDuration);
        pos.x = ctx.boss->getBasePosition().x - std::sin(angle) * 1000.f;
        pos.y = fixedY;
    }
    else {
        pos.x = ctx.boss->getBasePosition().x;
        pos.y = ctx.boss->getBasePosition().y;
        isCompleted = true;
    }

    ctx.boss->setPosition(pos);
}
void SunflowerLungeState::exit() {}