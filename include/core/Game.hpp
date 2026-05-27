#pragma once
#include "entities/Player.hpp"
#include "core/Platform.hpp"
#include "entities/Enemy.hpp"     
#include "entities/Minions.hpp"   
#include <vector>
#include <string>
#include <memory> 

class Game {
private:
    sf::Vector2u WindowResolution;
    sf::RenderWindow GameWindow;
    sf::Clock Timer;
    sf::Time timeSinceLastUpdate;
    sf::Time TimePerFrame;
    std::vector<int> entityList;

    Player mPlayer;
    std::vector<Platform> mPlatforms;

    std::vector<std::unique_ptr<Enemy>> mEnemies;
    sf::Time mSpawnTimer;

    sf::View mGameView;
    sf::Vector2f mLevelLimits;

    const float mLevelWidth = 3000.f;
    const float mLevelHeight = 600.f;

    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;

    sf::Texture mTextureGround;
    sf::Texture mTextureLeaf;
    sf::Texture mTexturePlayer;
    sf::Texture mTextureSunflower;
    sf::Texture mTextureHoming;
    sf::Texture mTextureFloor;
    sf::Texture mTextureFlying;

public:
    Game();
    Game(sf::Vector2u WindowResolution);
    Game(const Game& other) = delete;
    ~Game();

    Game& operator=(const Game& other) = delete;
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();

    void loadLevel(const std::string& filename);
    void updateCamera(sf::Time deltaTime);

    void spawnRandomMinion();
    void handleCollisions();
};