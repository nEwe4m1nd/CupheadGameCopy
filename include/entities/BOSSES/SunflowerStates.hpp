#pragma once
#include "entities/EnemyProjectile.hpp"
#include <SFML/System/Time.hpp>
#include <string>
#include <vector>
#include <memory>

class SunflowerBoss;
class Player;

struct BossContext {
    SunflowerBoss* boss;
    Player* player; // Нужен для определения координаты Y при бумеранге и выпадах
};

class BossState {
protected:
    BossContext ctx;
    float stateTimer = 0.0f;
    bool isCompleted = false;
public:
    explicit BossState(BossContext context) : ctx(context) {}
    virtual ~BossState() = default;
    virtual void enter() = 0;
    virtual void update(float dt) { stateTimer += dt; }
    virtual void exit() = 0;
    [[nodiscard]] bool isFinished() const { return isCompleted; }
};

class SunflowerIdleState : public BossState {
public:
    using BossState::BossState;
    void enter() override;
    void update(float dt) override;
    void exit() override;
};

// 1. Атака желудями (Acorns)
class SunflowerAcornsState : public BossState {
private:
    std::vector<int> fireOrder;
    int shotsFired = 0;
    float fireTimer = 0.0f;
public:
    using BossState::BossState;
    void enter() override;
    void update(float dt) override;
    void exit() override;
};

// 2. Гатлинг (Снаряды с неба)
class SunflowerGatlingState : public BossState {
private:
    float shootTimer = 0.0f;
    int shotsFired = 0; 
public:
    using BossState::BossState;
    void enter() override;
    void update(float dt) override;
    void exit() override;
};

// 3. Бумеранг (Boomerang)
class SunflowerBoomerangState : public BossState {
private:
    bool returningPhase = false;
public:
    using BossState::BossState;
    void enter() override;
    void update(float dt) override;
    void exit() override;
};

// 4. Лозы с миньонами (Vines)
class SunflowerVinesState : public BossState {
private:
    int vinesSpawned = 0;
    float vineTimer = 0.0f;
public:
    using BossState::BossState;
    void enter() override;
    void update(float dt) override;
    void exit() override;
};

// 5. Выпад головой (Lunge)
class SunflowerLungeState : public BossState {
private:
    bool isLowAttack = false;
public:
    using BossState::BossState;
    void enter() override;
    void update(float dt) override;
    void exit() override;
};