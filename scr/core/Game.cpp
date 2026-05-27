#include "core/Game.hpp"
#include "entities/BOSSES/SunflowerBoss.hpp"
#include <iostream>
#include <optional>
#include <fstream>
#include <sstream>
#include <algorithm> // Для std::remove_if

const sf::Vector2u Resolution_HD(1280u, 720u);
const sf::Vector2u Resolution_FHD(1920u, 1080u);

Game::Game()
    : mState(GameState::Playing)
    , Timer()
    , timeSinceLastUpdate(sf::Time::Zero)
    , TimePerFrame(sf::seconds(1.f / 60.f))
    , mSpawnTimer(sf::Time::Zero)
    , mBackgroundTexture()
    , mBackgroundSprite(mBackgroundTexture)
{
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

    // Создаем обычное окно под разрешение рабочего стола
    GameWindow.create(desktopMode, "Cuphead Game", sf::Style::Default);
    GameWindow.setFramerateLimit(60);

    // Размер области видимости камеры на экране
    mGameView.setSize({ 1920.f, 1080.f });

    // Заменяем mLevelLimits на размер одного экрана
    mLevelLimits = { 1920.f, 1080.f };
    mGameView.setCenter({ 1920.f / 2.f, 1080.f / 2.f });

    // 1. Загрузка фоновой текстуры
    if (mBackgroundTexture.loadFromFile("assets/forest_bg.jpg")) {
        mBackgroundSprite.setTexture(mBackgroundTexture, true);

        sf::Vector2u textureSize = mBackgroundTexture.getSize();
        float scaleX = 1920.f / textureSize.x;
        float scaleY = 1080.f / textureSize.y;
        mBackgroundSprite.setScale({ scaleX, scaleY });
    }

    // 2. Централизованная загрузка текстур для платформ во избежание Assertion failed
    if (!mTextureGround.loadFromFile("assets/ground.png")) {
        std::cerr << "Failed to load assets/ground.png" << std::endl;
    }
    if (!mTextureLeaf.loadFromFile("assets/leaf.png")) {
        std::cerr << "Failed to load assets/leaf.png" << std::endl;
    }
    mTextureBullet.loadFromFile("assets/peashot.png");
    mTextureSuper.loadFromFile("assets/peashot_EX.png");

    if (!mFont.openFromFile("assets/fronts/Gatok.otf")) {
        std::cerr << "Failed to load font" << std::endl;
    }
    else {
        mDeathText.emplace(mFont, "YOU DIED", 120);

        mDeathText->setFillColor(sf::Color::Red);
        mDeathText->setStyle(sf::Text::Bold);
        mDeathText->setPosition({ 650.f, 400.f });
    }

    // Загрузка уровня и передача платформ игроку
    loadLevel("include/levels/testLevel.txt");
    mPlayer.setPlatforms(mPlatforms);

    // ИСПРАВЛЕНИЕ: Появление игрока на земле в начале битвы
    mPlayer.setPosition({ 0, 650 });

    srand(static_cast<unsigned>(time(nullptr)));
}

Game::Game(sf::Vector2u windowResolution)
    : mState(GameState::Playing)
    , GameWindow(sf::VideoMode(windowResolution), "Cuphead Clone Project")
    , Timer()
    , timeSinceLastUpdate(sf::Time::Zero)
    , TimePerFrame(sf::seconds(1.f / 60.f))
    , mLevelLimits(0.f, 0.f)
    , mSpawnTimer(sf::Time::Zero)
    , mBackgroundTexture()
    , mBackgroundSprite(mBackgroundTexture)
{
    mLevelLimits = { 1920.f, 1080.f };
    mGameView.setCenter({ 1920.f / 2.f, 1080.f / 2.f });

    if (mBackgroundTexture.loadFromFile("assets/forest_bg.jpg")) {
        mBackgroundSprite.setTexture(mBackgroundTexture, true);

        sf::Vector2u textureSize = mBackgroundTexture.getSize();
        float scaleX = 1920.f / textureSize.x;
        float scaleY = 1080.f / textureSize.y;
        mBackgroundSprite.setScale({ scaleX, scaleY });
    }

    // Централизованная загрузка текстур для платформ во избежание Assertion failed
    if (!mTextureGround.loadFromFile("assets/ground.png")) {
        std::cerr << "Failed to load assets/ground.png" << std::endl;
    }
    if (!mTextureLeaf.loadFromFile("assets/leaf.png")) {
        std::cerr << "Failed to load assets/leaf.png" << std::endl;
    }

    if (!mFont.openFromFile("assets/fronts/Gatok.otf")) {
        std::cerr << "Failed to load font" << std::endl;
    }
    else {
        mDeathText.emplace(mFont, "YOU DIED", 120);

        mDeathText->setFillColor(sf::Color::Red);
        mDeathText->setStyle(sf::Text::Bold);
        mDeathText->setPosition({ 650.f, 400.f });
    }

    GameWindow.setFramerateLimit(60);
    mGameView.setSize(static_cast<sf::Vector2f>(windowResolution));

    loadLevel("include/levels/testLevel.txt");
    mPlayer.setPlatforms(mPlatforms);

    // ИСПРАВЛЕНИЕ: Появление игрока на земле в начале битвы
    mPlayer.setPosition({ 0.f, 650.f });

    srand(static_cast<unsigned>(time(nullptr)));
}

Game::~Game() {}

void Game::loadLevel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open level file " << filename << std::endl;
        return;
    }

    mPlatforms.clear();
    mEnemies.clear();

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '/') continue;

        std::stringstream ss(line);
        std::string identifier;

        if (ss >> identifier) {
            // 1. ПАРСИМ ПЛАТФОРМЫ
            if (identifier == "0" || identifier == "1" || identifier == "2" || identifier == "3") {
                int typeInt = std::stoi(identifier);
                float x, y, w, h, offX, offY, speed;

                if (ss >> x >> y >> w >> h >> offX >> offY >> speed) {
                    PlatformType type = static_cast<PlatformType>(typeInt);

                    // Выбираем текстуру в зависимости от типа платформы
                    // (Предполагается, что конструктор Platform теперь принимает ссылку на текстуру)

                    const sf::Texture& currentTex = (type == PlatformType::Solid) ? mTextureGround : mTextureLeaf;

                    mPlatforms.emplace_back(
                        sf::Vector2f(x, y), sf::Vector2f(w, h), type, currentTex, sf::Vector2f(offX, offY), speed
                    );
                }
            }
            // 2. ПАРСИМ БОССА
            else if (identifier == "Boss") {
                std::string bossName;
                float x, y;
                if (ss >> bossName >> x >> y) {
                    if (bossName == "Sunflower") {
                        mEnemies.push_back(std::make_unique<SunflowerBoss>(sf::Vector2f(x, y)));
                    }
                }
            }
            // 3. ПАРСИМ МИНЬОНОВ
            else if (identifier == "Enemy") {
                std::string enemyType;
                float x, y;
                if (ss >> enemyType >> x >> y) {
                    if (enemyType == "Homing") mEnemies.push_back(std::make_unique<HomingChomper>(sf::Vector2f(x, y)));
                    else if (enemyType == "Floor") mEnemies.push_back(std::make_unique<FloorChomper>(sf::Vector2f(x, y), -1.f));
                    else if (enemyType == "Flying") mEnemies.push_back(std::make_unique<FlyingChomper>(sf::Vector2f(x, y)));
                }
            }
        }
    }
    file.close();

    // --- ЛОГИКА ИСКЛЮЧЕНИЯ: ПРИОРИТЕТ БОССА ---
    bool hasBoss = false;
    for (const auto& enemy : mEnemies) {
        if (dynamic_cast<SunflowerBoss*>(enemy.get()) != nullptr) {
            hasBoss = true;
            break;
        }
    }

    if (hasBoss) {
        mEnemies.erase(std::remove_if(mEnemies.begin(), mEnemies.end(),
            [](const std::unique_ptr<Enemy>& e) {
                return dynamic_cast<SunflowerBoss*>(e.get()) == nullptr;
            }), mEnemies.end());
    }
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

        if (event->is<sf::Event::KeyPressed>()) {

            const auto* keyEvent = event->getIf<sf::Event::KeyPressed>();

            if (keyEvent->code == sf::Keyboard::Key::R &&
                (mState == GameState::GameOver || mState == GameState::Victory)) {
                restartGame();
            }
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

        // Коллизия пуль игрока с врагами
        for (auto& bullet : bullets) {
            if (bullet.isActive() && bullet.getBounds().findIntersection(enemyBounds).has_value()) {
                enemy->takeDamage(bullet.getDamage());
                bullet.destroy();
            }
        }

        // Коллизия супер-атак игрока с врагами
        for (auto& super : supers) {
            if (super.isActive() && super.getBounds().findIntersection(enemyBounds).has_value()) {
                enemy->takeDamage(super.getDamage());
            }
        }

        // ИСПРАВЛЕНИЕ КОЛЛИЗИЙ: Игрок просто получает урон и проходит СКВОЗЬ врагов (триггерная коллизия)
        if (playerBounds.findIntersection(enemyBounds).has_value()) {
            mPlayer.takeDamage(1);

            // Если это обычный миньон, он исчезает при столкновении
            if (dynamic_cast<SunflowerBoss*>(enemy.get()) == nullptr) {
                enemy->destroy();
            }
        }

        // Снаряды летающего миньона
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

        // Снаряды босса и логика парирования
        SunflowerBoss* boss = dynamic_cast<SunflowerBoss*>(enemy.get());
        if (boss != nullptr) {
            auto& bossProjectiles = boss->getProjectiles();

            for (auto& proj : bossProjectiles) {
                if (proj.isActive() && proj.getBounds().findIntersection(playerBounds).has_value()) {
                    // Если пуля РОЗОВАЯ и игрок движется вниз (парирование)
                    if (proj.isParryable() && mPlayer.getVelocityY() > 0.f) {
                        mPlayer.setVelocityY(-400.f); // Отскок вверх
                        proj.destroy();
                    }
                    else {
                        mPlayer.takeDamage(static_cast<int>(proj.getDamage()));
                        proj.destroy();
                    }
                }
            }
        }
    }
}

void Game::update(sf::Time deltaTime) {

    // ===== STATE CHECK: GAME OVER / VICTORY =====
    if (mState == GameState::GameOver || mState == GameState::Victory) {
        return;
    }

    // ===== PLAYER DEATH CHECK =====
    if (mPlayer.isDead() && mState == GameState::Playing) {
        mState = GameState::GameOver;
        return;
    }

    // ===== PLATFORMS =====
    for (auto& platform : mPlatforms) {
        platform.update(deltaTime);
    }

    // ===== PLAYER =====
    mPlayer.update(deltaTime);

    // ===== SPAWN LOGIC (ONLY IF NOT BOSS FIGHT) =====
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

    // ===== ENEMIES UPDATE =====
    for (auto& enemy : mEnemies) {
        enemy->update(deltaTime, mPlayer.getPosition());
    }

    // ===== COLLISIONS =====
    handleCollisions();

    // ===== REMOVE DEAD ENEMIES =====
    mEnemies.erase(std::remove_if(mEnemies.begin(), mEnemies.end(),
        [](const std::unique_ptr<Enemy>& e) {
            return !e->isActive();
        }), mEnemies.end());

    // ===== VICTORY CHECK =====
    bool hasEnemies = false;

    for (const auto& enemy : mEnemies) {
        if (enemy->isActive()) {
            hasEnemies = true;
            break;
        }
    }

    if (!hasEnemies && mState == GameState::Playing) {
        mState = GameState::Victory;
        return;
    }

    // ===== CAMERA =====
    updateCamera(deltaTime);
}

void Game::render() {

    GameWindow.clear(sf::Color::Black);
    GameWindow.setView(mGameView);

    // ===== WORLD RENDER (игровой мир) =====
    if (mState == GameState::Playing) {

        // фон
        GameWindow.draw(mBackgroundSprite);

        // платформы
        for (auto& platform : mPlatforms) {
            platform.draw(GameWindow);
        }

        // враги
        for (auto& enemy : mEnemies) {
            enemy->draw(GameWindow);
        }

        // игрок
        mPlayer.draw(GameWindow);
    }

    // ===== DEATH SCREEN =====
    if (mState == GameState::GameOver) {

        GameWindow.setView(GameWindow.getDefaultView());

        sf::RectangleShape overlay;
        overlay.setSize(sf::Vector2f(GameWindow.getSize()));
        overlay.setFillColor(sf::Color(0, 0, 0, 220));
        GameWindow.draw(overlay);

        if (mDeathText.has_value()) {

            sf::FloatRect bounds = mDeathText->getLocalBounds();

            mDeathText->setOrigin({
                bounds.position.x + bounds.size.x / 2.f,
                bounds.position.y + bounds.size.y / 2.f
                });

            mDeathText->setPosition({
                GameWindow.getSize().x / 2.f,
                GameWindow.getSize().y / 2.f - 100.f
                });

            GameWindow.draw(*mDeathText);
        }

        sf::Text restartText(mFont, "PRESS R TO RESTART", 48);
        restartText.setFillColor(sf::Color::White);

        sf::FloatRect restartBounds = restartText.getLocalBounds();
        restartText.setOrigin({
            restartBounds.position.x + restartBounds.size.x / 2.f,
            restartBounds.position.y + restartBounds.size.y / 2.f
            });

        restartText.setPosition({
            GameWindow.getSize().x / 2.f,
            GameWindow.getSize().y / 2.f + 80.f
            });

        GameWindow.draw(restartText);
    }

    // ===== VICTORY SCREEN =====
    if (mState == GameState::Victory) {

        GameWindow.setView(GameWindow.getDefaultView());

        sf::RectangleShape overlay;
        overlay.setSize(sf::Vector2f(GameWindow.getSize()));
        overlay.setFillColor(sf::Color(0, 0, 0, 220));
        GameWindow.draw(overlay);

        sf::Text winText(mFont, "YOU WIN!", 120);
        winText.setFillColor(sf::Color::Yellow);

        sf::FloatRect bounds = winText.getLocalBounds();
        winText.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
            });

        winText.setPosition({
            GameWindow.getSize().x / 2.f,
            GameWindow.getSize().y / 2.f - 100.f
            });

        GameWindow.draw(winText);

        sf::Text restartText(mFont, "PRESS R TO RESTART", 48);
        restartText.setFillColor(sf::Color::White);

        sf::FloatRect restartBounds = restartText.getLocalBounds();
        restartText.setOrigin({
            restartBounds.position.x + restartBounds.size.x / 2.f,
            restartBounds.position.y + restartBounds.size.y / 2.f
            });

        restartText.setPosition({
            GameWindow.getSize().x / 2.f,
            GameWindow.getSize().y / 2.f + 80.f
            });

        GameWindow.draw(restartText);
    }

    GameWindow.display();
}

void Game::restartGame() {

    mState = GameState::Playing;

    mEnemies.clear();
    mPlatforms.clear();

    loadLevel("include/levels/testLevel.txt");

    mPlayer.reset();
    mPlayer.setPlatforms(mPlatforms);

    mPlayer.setPosition({ 0.f, 650.f });

    mGameView.setCenter({ 1920.f / 2.f, 1080.f / 2.f });

    mSpawnTimer = sf::Time::Zero;
}
