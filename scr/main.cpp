#include "core/Game.hpp"

int main() {
    //запуск по умолчанию (HD)
    Game game;

    // запуск с кастомным разрешением
    // Game game(sf::Vector2u(1920u, 1080u)); 

    game.run();
    return 0;
}