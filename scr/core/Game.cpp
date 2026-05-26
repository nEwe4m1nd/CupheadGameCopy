#include "core/Game.hpp"
#include "entities/BOSSES/SunflowerBoss.hpp"
#include <iostream>
#include <optional>
#include <fstream>
#include <sstream>

const sf::Vector2u Resolution_HD(1280u, 720u);
const sf::Vector2u Resolution_FHD(1920u, 1080u);

Game::Game()
    : Timer()
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
    mGameView.setCenter({ 1920.f / 2.f, 1080.f / 2.f });

    // ОШИБКА БЫЛА ТУТ: Границы уровня должны быть шире, чем экран (1920),
    // чтобы камера могла ехать вправо. Поставим длину 3000 (как mLevelWidth в хэдере)
    mLevelLimits = { 3000.f, 1080.f };

    // ЯВНО загружаем и натягиваем текстуру на всю длину и высоту игрового мира
    if (mBackgroundTexture.loadFromFile("assets/forest_bg.jpg")) {
        mBackgroundTexture.setRepeated(true);
        mBackgroundSprite.setTexture(mBackgroundTexture);

        // Натягиваем текстуру от 0 до 3000 по ширине и до 1080 по высоте камеры
        mBackgroundSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 3000, 1080 }));
    }

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
    if (mBackgroundTexture.loadFromFile("assets/forest_bg.jpg")) {
        mBackgroundSprite.setTexture(mBackgroundTexture);

        sf::Vector2u textureSize = mBackgroundTexture.getSize();
        float scaleX = 1920.f / textureSize.x;
        float scaleY = 1080.f / textureSize.y;

        mBackgroundSprite.setScale({ scaleX, scaleY });
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
        std::cerr << "Error: Could not open level file " << filename << std::endl;
        return;
    }

    mPlatforms.clear();
    mEnemies.clear(); // Очищаем старых врагов перед загрузкой

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '/') continue;

        std::stringstream ss(line);
        std::string identifier;

        // Читаем первое слово в строке
        if (ss >> identifier) {

            // 1. ПАРСИМ ПЛАТФОРМЫ (если строка начинается с "0", "1" или "2")
            if (identifier == "0" || identifier == "1" || identifier == "2") {
                int typeInt = std::stoi(identifier);
                float x, y, w, h, offX, offY, speed;

                if (ss >> x >> y >> w >> h >> offX >> offY >> speed) {
                    PlatformType type = static_cast<PlatformType>(typeInt);
                    mPlatforms.emplace_back(
                        sf::Vector2f(x, y), sf::Vector2f(w, h), type, sf::Vector2f(offX, offY), speed
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

    // Если на уровне есть босс, стираем из вектора всех, кто боссом не является
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

        SunflowerBoss* boss = dynamic_cast<SunflowerBoss*>(enemy.get());
        if (boss != nullptr) {
            auto& bossProjectiles = boss->getProjectiles();
            sf::FloatRect playerBounds = mPlayer.getBounds();

            for (auto& proj : bossProjectiles) {
                if (proj.isActive() && proj.getBounds().findIntersection(playerBounds).has_value()) {

                    // Если пуля РОЗОВАЯ и игрок ПАДАЕТ ВНИЗ (шлепок/двойной прыжок)
                    if (proj.isParryable() && mPlayer.getVelocityY() > 0.f) {
                        // ПАРИРОВАНИЕ УСПЕШНО
                        // mPlayer.heal(1);       // Раскомментируй, если у тебя есть метод лечения
                        mPlayer.setVelocityY(-400.f); // Подкидываем игрока обратно в воздух (отскок)
                        proj.destroy();           // Розовая пуля исчезает
                    }
                    else {
                        // ОБЫЧНОЕ ПОПАДАНИЕ (пуля не розовая ИЛИ игрок просто в неё врезался)
                        mPlayer.takeDamage(static_cast<int>(proj.getDamage()));
                        proj.destroy();           // Пуля исчезает после нанесения урона
                    }

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
    GameWindow.clear(sf::Color::Black);

    // 1. Рисуем фон в "координатах окна" (без учета игровой камеры)
    GameWindow.setView(GameWindow.getDefaultView());
    GameWindow.draw(mBackgroundSprite);

    // 2. Включаем игровую камеру для всех остальных объектов (игрок, босс, платформы)
    GameWindow.setView(mGameView);

    for (auto& platform : mPlatforms) {
        platform.draw(GameWindow);
    }

    for (auto& enemy : mEnemies) {
        enemy->draw(GameWindow);
    }

    mPlayer.draw(GameWindow);

    GameWindow.display();
}