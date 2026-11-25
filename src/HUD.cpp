#include "HUD.hpp"

#include "Types.hpp"

#include <algorithm>
#include <sstream>
#include <string>

namespace space_survivor {
namespace {
void drawLabel(sf::RenderTarget& target, const sf::Font& font, bool hasFont,
               const std::string& value, unsigned int size,
               const sf::Vector2f& position, const sf::Color& color) {
  if (!hasFont) {
    return;
  }
  sf::Text text(value, font, size);
  text.setFillColor(color);
  text.setPosition(position);
  target.draw(text);
}

void drawHealth(sf::RenderTarget& target, const Player& player) {
  for (int idx = 0; idx < player.getMaxHealth(); ++idx) {
    sf::RectangleShape life({23.0F, 8.0F});
    life.setPosition(1108.0F + static_cast<float>(idx) * 29.0F, 14.0F);
    life.setFillColor(idx < player.getHealth() ? sf::Color(65, 245, 185)
                          : sf::Color(55, 65, 85));
    life.setOutlineThickness(1.0F);
    life.setOutlineColor(sf::Color(130, 170, 195));
    target.draw(life);
  }
}

void drawStatusBars(sf::RenderTarget& target, const Player& player) {
  if (player.hasRapidFire()) {
    sf::RectangleShape bar({std::min(160.0F, player.getRapidFireTime() * 16.0F), 4.0F});
    bar.setPosition(1060.0F, 38.0F);
    bar.setFillColor(sf::Color(255, 75, 220));
    target.draw(bar);
  }
  if (player.hasShield()) {
    sf::RectangleShape bar({std::min(160.0F, player.getShieldTime() * 20.0F), 4.0F});
    bar.setPosition(1060.0F, 46.0F);
    bar.setFillColor(sf::Color(70, 175, 255));
    target.draw(bar);
  }
}

void drawBossBar(sf::RenderTarget& target, const sf::Font& font, bool hasFont,
                 int health, int maxHealth) {
  const float ratio = maxHealth > 0
              ? static_cast<float>(std::clamp(health, 0, maxHealth)) /
                 static_cast<float>(maxHealth)
              : 0.0F;
  sf::RectangleShape background({420.0F, 13.0F});
  background.setPosition(430.0F, 76.0F);
  background.setFillColor(sf::Color(50, 20, 45));
  target.draw(background);

  sf::RectangleShape fill({420.0F * ratio, 13.0F});
  fill.setPosition(430.0F, 76.0F);
  fill.setFillColor(sf::Color(255, 55, 145));
  target.draw(fill);
  drawLabel(target, font, hasFont, "SECTOR WARDEN", 12U,
       {580.0F, 91.0F}, sf::Color(255, 165, 215));
}
}  // namespace

void HUD::draw(sf::RenderTarget& target, const sf::Font& font, bool hasFont,
               const Player& player, int score, int highScore,
               float survivalTime, int wave, bool bossActive,
               int bossHealth, int bossMaxHealth) const {
  sf::RectangleShape panel({static_cast<float>(config::WindowWidth), 58.0F});
  panel.setFillColor(sf::Color(4, 9, 24, 225));
  target.draw(panel);

  drawLabel(target, font, hasFont, "SCORE  " + std::to_string(score), 17U,
       {24.0F, 9.0F}, sf::Color(90, 235, 255));
  drawLabel(target, font, hasFont, "BEST  " + std::to_string(highScore), 14U,
       {24.0F, 33.0F}, sf::Color(145, 165, 195));
  drawLabel(target, font, hasFont, "WAVE  " + std::to_string(wave), 17U,
       {400.0F, 9.0F}, sf::Color(200, 135, 255));

  const int totalSeconds = std::max(0, static_cast<int>(survivalTime));
  const int minutes = totalSeconds / 60;
  const int seconds = totalSeconds % 60;
  std::ostringstream timer;
  timer << (minutes < 10 ? "0" : "") << minutes << ":"
     << (seconds < 10 ? "0" : "") << seconds;
  drawLabel(target, font, hasFont, "TIME  " + timer.str(), 14U,
       {400.0F, 33.0F}, sf::Color(145, 165, 195));
  drawLabel(target, font, hasFont, "HULL", 13U,
       {1060.0F, 9.0F}, sf::Color(170, 190, 220));

  drawHealth(target, player);
  drawStatusBars(target, player);
  if (bossActive) {
    drawBossBar(target, font, hasFont, bossHealth, bossMaxHealth);
  }
}

}  // namespace space_survivor
