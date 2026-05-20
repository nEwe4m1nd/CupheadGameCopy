#pragma once
#include "include.hpp"
#include "core/Platform.hpp"
#include "entities/Player.hpp"
#include "entities/Enemy.hpp"
#include "entities/Minions.hpp"
#include <ctime>
#include <vector>
#include <memory>

class Game {
private:
    sf::RenderWindow GameWindow;
    sf::View mGameView;
    sf::Clock Timer;
    sf::Time timeSinceLastUpdate;
    const sf::Time TimePerFrame;

    sf::Vector2f mLevelLimits;
    std::vector<Platform> mPlatforms;
    Player mPlayer;

    std::vector<std::unique_ptr<Enemy>> mEnemies;
    sf::Time mSpawnTimer;
    const sf::Time SPAWN_COOLDOWN = sf::seconds(4.0f);

public:
    Game();
    Game(sf::Vector2u WindowResolution);
    Game(const Game& other) = delete; //     сфига-то нельз€, в SFML нет конструкторов копировани€ Time и Clock
    ~Game();

public:
    Game& operator=(const Game& other) = delete;
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void spawnRandomMinion();
    void handleCollisions();

private:
    void loadLevel(const std::string& filename);
    void updateCamera(sf::Time deltaTime);
};
