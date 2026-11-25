#include "Game.hpp"

#include <exception>
#include <iostream>

int main() {
  try {
    space_survivor::Game game;
    game.run();
  } catch (const std::exception& exception) {
    std::cerr << "SPACE SURVIVOR failed to start: " << exception.what() << '\n';
    return 1;
  }
  return 0;
}
