#include "core/Game.hpp"
#include <optional>


const sf::Vector2u Resolution_HD(1280u, 720u);
const sf::Vector2u Resolution_FHD(1920u, 1080u);


Game::Game()
    : GameWindow(sf::VideoMode({ 800u, 600u }), "Cuphead Game"),
    Timer(),
    timeSinceLastUpdate(sf::Time::Zero),
    TimePerFrame(sf::seconds(1.f / 60.f))
{
    GameWindow.setFramerateLimit(60);

    mPlatforms.emplace_back(sf::Vector2f{ 0.f, 550.f }, sf::Vector2f{ 800.f, 50.f }, PlatformType::Solid);

    mPlatforms.emplace_back(sf::Vector2f{ 560.f, 420.f }, sf::Vector2f{ 70.f, 170.f }, PlatformType::Solid);

    mPlatforms.emplace_back(sf::Vector2f{ 250.f, 300.f }, sf::Vector2f{ 200.f, 20.f }, PlatformType::OneWay);

    mPlayer.setPlatforms(mPlatforms);
}


Game::Game(sf::Vector2u windowResolution)
    : WindowResolution(windowResolution),
    GameWindow(sf::VideoMode(WindowResolution), "Cuphead Clone Project"),
    Timer(),
    timeSinceLastUpdate(sf::Time::Zero),
    TimePerFrame(sf::seconds(1.f / 60.f))
{
    GameWindow.setFramerateLimit(60);
}


Game::~Game() {
}



// главный игровой цикл
void Game::run() {
    while (GameWindow.isOpen()) {
        sf::Time deltaTime = Timer.restart();
        timeSinceLastUpdate += deltaTime;

        while (timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            update(TimePerFrame);
        }

        // анортизация
        render();
    }
}

// ввод
void Game::processEvents() {
    while (const std::optional<sf::Event> event = GameWindow.pollEvent()) {

        if (event->is<sf::Event::Closed>()) {
            GameWindow.close();
        }
    }
}

// обновление логики
void Game::update(sf::Time deltaTime) {
    mPlayer.update(deltaTime);
}

// отрисовка
void Game::render() {
    GameWindow.clear(sf::Color(40, 40, 40));

    for (const auto& platform : mPlatforms) {
        platform.draw(GameWindow);
    }

    mPlayer.draw(GameWindow);

    GameWindow.display();
}