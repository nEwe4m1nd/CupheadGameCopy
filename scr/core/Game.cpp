#include "core/Game.hpp"
#include <iostream>
#include <optional>
#include <fstream>
#include <sstream>


const sf::Vector2u Resolution_HD(1280u, 720u);
const sf::Vector2u Resolution_FHD(1920u, 1080u);


Game::Game()
    : GameWindow(sf::VideoMode({ 800u, 600u }), "Cuphead Game")
    , Timer()
    , timeSinceLastUpdate(sf::Time::Zero)
    , TimePerFrame(sf::seconds(1.f / 60.f))
    , mLevelLimits(0.f, 0.f)
{
    mGameView.setSize({ 800.f, 600.f });

    loadLevel("include/levels/testLevel.txt");

    mPlayer.setPlatforms(mPlatforms);
}


Game::Game(sf::Vector2u windowResolution)
    : WindowResolution(windowResolution)
    , GameWindow(sf::VideoMode(WindowResolution), "Cuphead Clone Project")
    , Timer()
    , timeSinceLastUpdate(sf::Time::Zero)
    , TimePerFrame(sf::seconds(1.f / 60.f))
    , mLevelLimits(0.f, 0.f)
{
    GameWindow.setFramerateLimit(60);
    mGameView.setSize(static_cast<sf::Vector2f>(WindowResolution));

    loadLevel("include/levels/testLevel.txt");

    mPlayer.setPlatforms(mPlatforms);
}


Game::~Game() {
}

void Game::loadLevel(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "DEBUG: file '" << filename << "' not found!\n";
        std::cout << "DEBUG: loading test level\n";
        
        //заглушка
        mPlatforms.emplace_back(sf::Vector2f{ 0.f, 550.f }, sf::Vector2f{ 3000.f, 50.f });
        mLevelLimits = { 3000.f, 600.f };
        return;
    }

    std::string line;
    float maxW = 800.f;
    float maxH = 600.f;
    int platformCount = 0;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        float x, y, width, height;

        if (ss >> x >> y >> width >> height) {
            mPlatforms.emplace_back(sf::Vector2f{ x, y }, sf::Vector2f{ width, height });
            platformCount++;

            if (x + width > maxW)  maxW = x + width;
            if (y + height > maxH) maxH = y + height;
        }
        else {
            std::cout << "DEBUG: error in reading line " << line << "\n";
        }
    }

    mLevelLimits = { maxW, maxH };
    file.close();

    std::cout << "DEBUG: LEVEL LOADED WITHOUT ANY PROBLEM\n";
    std::cout << "DEBUG: platform found" << platformCount << "\n";
    std::cout << "DEBUG: camera limits X=" << maxW << " Y=" << maxH << "\n";
}

void Game::updateCamera(sf::Time deltaTime) {
    sf::Vector2f playerPos = mPlayer.getPosition();
    sf::Vector2f viewSize = mGameView.getSize();
    sf::Vector2f currentCenter = mGameView.getCenter();

    float lerpSpeed = 4.f;
    sf::Vector2f targetCenter = currentCenter;

    targetCenter.x = currentCenter.x + (playerPos.x - currentCenter.x) * lerpSpeed * deltaTime.asSeconds();
    targetCenter.y = currentCenter.y + (playerPos.y - currentCenter.y) * lerpSpeed * deltaTime.asSeconds();

    float minX = viewSize.x / 2.f;
    float maxX = mLevelLimits.x - (viewSize.x / 2.f);
    float minY = viewSize.y / 2.f;
    float maxY = mLevelLimits.y - (viewSize.y / 2.f);

    if (maxX < minX) maxX = minX;
    if (maxY < minY) maxY = minY;

    targetCenter.x = std::clamp(targetCenter.x, minX, maxX);
    targetCenter.y = std::clamp(targetCenter.y, minY, maxY);

    mGameView.setCenter(targetCenter);
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
    updateCamera(deltaTime);
}

// отрисовка
void Game::render() {
    GameWindow.clear(sf::Color(40, 40, 40));
    GameWindow.setView(mGameView);

    // отрисовка игровых объектов
    for (const auto& platform : mPlatforms) {
        platform.draw(GameWindow);
    }
    mPlayer.draw(GameWindow);

    GameWindow.display();
}
