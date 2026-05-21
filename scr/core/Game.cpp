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
<<<<<<< Updated upstream
        std::cout << "DEBUG: loading test level\n";
        
        //Ã§Ã Ã£Ã«Ã³Ã¸ÃªÃ 
        mPlatforms.emplace_back(sf::Vector2f{ 0.f, 550.f }, sf::Vector2f{ 3000.f, 50.f }, PlatformType::Solid);
        mLevelLimits = { 3000.f, 600.f };
=======
>>>>>>> Stashed changes
        return;
    }

    mPlatforms.clear();
    mEnemies.clear();

    std::string line;
    float maxW = 800.f;
    float maxH = 600.f;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string objectType;
        ss >> objectType;

<<<<<<< Updated upstream
        // Ã—Ã¨Ã²Ã Ã¥Ã¬ 4 Ã·Ã¨Ã±Ã«Ã  Ã¨ Ã±Ã²Ã°Ã®ÃªÃ³-Ã²Ã¨Ã¯
        if (ss >> x >> y >> w >> h >> typeStr) {
            PlatformType type = PlatformType::Solid; // ÃÃ® Ã³Ã¬Ã®Ã«Ã·Ã Ã­Ã¨Ã¾
=======
        // ÏÀÐÑÈÍÃ ÑÒÀÒÈ×ÍÎÉ ÏËÀÒÔÎÐÌÛ
        if (objectType == "Platform") {
            float x, y, w, h;
            std::string typeStr;
>>>>>>> Stashed changes

            if (ss >> x >> y >> w >> h >> typeStr) {
                PlatformType type = PlatformType::Solid;
                if (typeStr == "OneWay") type = PlatformType::OneWay;
                else if (typeStr == "Death")  type = PlatformType::Death;

                mPlatforms.emplace_back(sf::Vector2f{ x, y }, sf::Vector2f{ w, h }, type);

                if (x + w > maxW) maxW = x + w;
                if (y + h > maxH) maxH = y + h;
            }
        }
        //  ÏÀÐÑÈÍÃ ÄÂÈÆÓÙÅÉÑß ÏËÀÒÔÎÐÌÛ
        else if (objectType == "Moving") {
            float x, y, w, h, moveX, moveY, speed;
            std::string typeStr;

            if (ss >> x >> y >> w >> h >> typeStr >> moveX >> moveY >> speed) {
                PlatformType type = PlatformType::Solid;
                if (typeStr == "OneWay") type = PlatformType::OneWay;
                else if (typeStr == "Death")  type = PlatformType::Death;

                mPlatforms.emplace_back(sf::Vector2f{ x, y }, sf::Vector2f{ w, h }, type, sf::Vector2f{ moveX, moveY }, speed);

                if (x + w + moveX > maxW) maxW = x + w + moveX;
                if (y + h + moveY > maxH) maxH = y + h + moveY;
            }
        }
        // ÏÀÐÑÈÍÃ ÑÓÙÍÎÑÒÅÉ
        else if (objectType == "Enemy") {
            std::string enemyType;
            float x, y;

            if (ss >> enemyType >> x >> y) {
                if (enemyType == "Homing") {
                    mEnemies.push_back(std::make_unique<HomingChomper>(sf::Vector2f(x, y)));
                }
                else if (enemyType == "Floor") {
                    mEnemies.push_back(std::make_unique<FloorChomper>(sf::Vector2f(x, y), -1.f));
                }
                else if (enemyType == "Flying") {
                    mEnemies.push_back(std::make_unique<FlyingChomper>(sf::Vector2f(x, y)));
                }
            }
        }
    }
    mLevelLimits = { maxW, maxH };
    file.close();
    std::cout << "DEBUG: level loaded. Camera limits X=" << maxW << " Y=" << maxH << "\n";
    std::cout << "DEBUG: Entities spawned: " << mEnemies.size() << "\n";
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

// Ã£Ã«Ã Ã¢Ã­Ã»Ã© Ã¨Ã£Ã°Ã®Ã¢Ã®Ã© Ã¶Ã¨ÃªÃ«
void Game::run() {
    while (GameWindow.isOpen()) {
        sf::Time deltaTime = Timer.restart();
        timeSinceLastUpdate += deltaTime;

        while (timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            update(TimePerFrame);
        }

        // Ã Ã­Ã®Ã°Ã²Ã¨Ã§Ã Ã¶Ã¨Ã¿
        render();
    }
}

// Ã¢Ã¢Ã®Ã¤
void Game::processEvents() {
    while (const std::optional<sf::Event> event = GameWindow.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            GameWindow.close();
        }
    }
}

// Ã®Ã¡Ã­Ã®Ã¢Ã«Ã¥Ã­Ã¨Ã¥ Ã«Ã®Ã£Ã¨ÃªÃ¨
void Game::update(sf::Time deltaTime) {
    for (auto& platform : mPlatforms) {
        platform.update(deltaTime);
    }
    mPlayer.update(deltaTime);
    updateCamera(deltaTime);
}

// Ã®Ã²Ã°Ã¨Ã±Ã®Ã¢ÃªÃ 
void Game::render() {
    GameWindow.clear(sf::Color(40, 40, 40));
    GameWindow.setView(mGameView);

    // Ã®Ã²Ã°Ã¨Ã±Ã®Ã¢ÃªÃ  Ã¨Ã£Ã°Ã®Ã¢Ã»Ãµ Ã®Ã¡ÃºÃ¥ÃªÃ²Ã®Ã¢
    for (const auto& platform : mPlatforms) {
        platform.draw(GameWindow);
    }
    mPlayer.draw(GameWindow);

    GameWindow.display();
}
