#pragma once
#include"include.hpp"

class Game {
private:
	sf::Vector2u WindowResolution;
	sf::RenderWindow GameWindow;
	sf::Clock Timer;
	sf::Time timeSinceLastUpdate;
	sf::Time TimePerFrame;
	std::vector<int> entityList;


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
};
