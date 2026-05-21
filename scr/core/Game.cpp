#include "core/Game.hpp"
#include "entities/BOSSES/SunflowerBoss.hpp"
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
    , mSpawnTimer(sf::Time::Zero)
    , mBackgroundTexture()
    , mBackgroundSprite(mBackgroundTexture)
{
    if (mBackgroundTexture.loadFromFile("assets/forest_bg.jpg")) {
        mBackgroundTexture.setRepeated(true);
        mBackgroundSprite.setTexture(mBackgroundTexture);
        mBackgroundSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 800, 720 }));
    }
    mGameView.setSize({ 800.f, 600.f });
    loadLevel("include/levels/testLevel.txt");
    mPlayer.setPlatforms(mPlatforms);
    srand(static_cast<unsigned>(time(nullptr)));
}

Game::Game(sf::Vector2u windowResolution)
    : GameWindow(sf::VideoMode(windowResolution), "Cuphead Clone Project")
    , Timer()
    , timeSinceLastUpdate(sf::Time::Zero)
    , TimePerFrame(sf::seconds(1.f / 60.f))
    , mLevelLimits(0.f, 0.f)
    , mSpawnTimer(sf::Time::Zero)
    , mBackgroundTexture()
    , mBackgroundSprite(mBackgroundTexture)
{
    if (mBackgroundTexture.loadFromFile("assets/forest_bg.png")) {
        mBackgroundTexture.setRepeated(true);
        mBackgroundSprite.setTexture(mBackgroundTexture);
        mBackgroundSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 800, 720 }));
    }
    GameWindow.setFramerateLimit(60);
    mGameView.setSize(static_cast<sf::Vector2f>(windowResolution));
    loadLevel("include/levels/testLevel.txt");
    mPlayer.setPlatforms(mPlatforms);
    srand(static_cast<unsigned>(time(nullptr)));
}

Game::~Game() {}

void Game::loadLevel(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "DEBUG: file '" << filename << "' not found!\n";
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

        if (objectType == "Platform") {
            float x, y, w, h;
            std::string typeStr;

            if (ss >> x >> y >> w >> h >> typeStr) {
                PlatformType type = PlatformType::Solid;
                if (typeStr == "OneWay") type = PlatformType::OneWay;
                else if (typeStr == "Death")  type = PlatformType::Death;

                mPlatforms.emplace_back(sf::Vector2f{ x, y }, sf::Vector2f{ w, h }, type);
                if (x + w > maxW) maxW = x + w;
                if (y + h > maxH) maxH = y + h;
            }
        }
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
                else if (enemyType == "Sunflower") {
                    mEnemies.push_back(std::make_unique<SunflowerBoss>(sf::Vector2f(x, y)));
                }
            }
        }
    }
    mLevelLimits = { maxW, maxH };
    file.close();
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

void Game::run() {
    while (GameWindow.isOpen()) {
        sf::Time deltaTime = Timer.restart();
        timeSinceLastUpdate += deltaTime;

        while (timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            update(TimePerFrame);
        }
        render();
    }
}

void Game::processEvents() {
    while (const std::optional<sf::Event> event = GameWindow.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            GameWindow.close();
        }
    }
}

void Game::spawnRandomMinion() {
    int type = rand() % 3;
    if (type == 0) mEnemies.push_back(std::make_unique<HomingChomper>(sf::Vector2f(1800.f, 200.f)));
    else if (type == 1) mEnemies.push_back(std::make_unique<FloorChomper>(sf::Vector2f(2000.f, 570.f), -1.f));
    else if (type == 2) mEnemies.push_back(std::make_unique<FlyingChomper>(sf::Vector2f(1600.f, 300.f)));
}

void Game::handleCollisions() {
    auto& bullets = mPlayer.getBullets();
    auto& supers = mPlayer.getSuperAttacks();
    sf::FloatRect playerBounds = mPlayer.getBounds();

    for (auto& enemy : mEnemies) {
        if (!enemy->isActive()) continue;
        sf::FloatRect enemyBounds = enemy->getBounds();

        for (auto& bullet : bullets) {
            if (bullet.isActive() && bullet.getBounds().findIntersection(enemyBounds).has_value()) {
                enemy->takeDamage(bullet.getDamage());
                bullet.destroy();
            }
        }

        for (auto& super : supers) {
            if (super.isActive() && super.getBounds().findIntersection(enemyBounds).has_value()) {
                enemy->takeDamage(super.getDamage());
            }
        }

        if (playerBounds.findIntersection(enemyBounds).has_value()) {
            mPlayer.takeDamage(1);

            if (dynamic_cast<SunflowerBoss*>(enemy.get()) == nullptr) {
                enemy->destroy();
            }
        }

        FlyingChomper* flyer = dynamic_cast<FlyingChomper*>(enemy.get());
        if (flyer != nullptr) {
            auto& enemyProjectiles = flyer->getProjectiles();
            for (auto& proj : enemyProjectiles) {
                if (proj.isActive() && proj.getBounds().findIntersection(playerBounds).has_value()) {
                    mPlayer.takeDamage(static_cast<int>(proj.getDamage()));
                    proj.destroy();
                }
            }
        }
    }
}

void Game::update(sf::Time deltaTime) {
    for (auto& platform : mPlatforms) {
        platform.update(deltaTime);
    }

    mPlayer.update(deltaTime);

    bool isBossFight = false;
    for (const auto& enemy : mEnemies) {
        if (dynamic_cast<SunflowerBoss*>(enemy.get()) != nullptr) {
            isBossFight = true;
            break;
        }
    }

    if (!isBossFight) {
        mSpawnTimer += deltaTime; 
        if (mSpawnTimer >= sf::seconds(4.0f)) {
            spawnRandomMinion();
            mSpawnTimer = sf::Time::Zero;
        }
    }

    for (auto& enemy : mEnemies) {
        enemy->update(deltaTime, mPlayer.getPosition());
    }

    handleCollisions();

    mEnemies.erase(std::remove_if(mEnemies.begin(), mEnemies.end(),
        [](const std::unique_ptr<Enemy>& e) { return !e->isActive(); }), mEnemies.end());

    updateCamera(deltaTime);
}


void Game::render() {
    GameWindow.clear(sf::Color(40, 40, 40));
    GameWindow.setView(mGameView);

    GameWindow.draw(mBackgroundSprite);

    for (const auto& platform : mPlatforms) {
        platform.draw(GameWindow);
    }

    for (const auto& enemy : mEnemies) {
        enemy->draw(GameWindow);
    }

    mPlayer.draw(GameWindow);
    GameWindow.display();
}