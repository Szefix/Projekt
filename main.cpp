#include <windows.h>
#include "SnakeGame.hpp"
#include <iostream>
#include <ctime>
#include <cstdlib>


int main() {
    srand(static_cast<unsigned int>(time(0)));
    SnakeGame game;
    game.run();
    return 0;
}
