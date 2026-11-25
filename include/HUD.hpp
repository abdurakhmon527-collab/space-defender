#pragma once

#include "Player.hpp"

#include <SFML/Graphics.hpp>

namespace space_survivor {

class HUD {
public:
  void draw(sf::RenderTarget& target, const sf::Font& font, bool hasFont,
       const Player& player, int score, int highScore,
       float survivalTime, int wave, bool bossActive,
       int bossHealth, int bossMaxHealth) const;
};

}  // namespace space_survivor
