#pragma once
#include "entities/Player.hpp"
#include "core/Platform.hpp"
#include <vector>
#include <string>

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

    sf::View mGameView;
    sf::Vector2f mLevelLimits;

	// ãðàíèöû óðîâíÿ
	const float mLevelWidth = 3000.f;
	const float mLevelHeight = 600.f;

public:
    Game();
    Game(sf::Vector2u WindowResolution);
    Game(const Game& other) = delete; //     ñôèãà-òî íåëüçÿ, â SFML íåò êîíñòðóêòîðîâ êîïèðîâàíèÿ Time è Clock
    ~Game();

public:
    Game& operator=(const Game& other) = delete;
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();

private:
    void loadLevel(const std::string& filename);
    void updateCamera(sf::Time deltaTime);
};
