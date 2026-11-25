#pragma once

#include "Projectile.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

namespace space_survivor {

class Boss {
public:
  void spawn(const sf::Vector2f& position) noexcept;
  void update(float deltaTime, const sf::Vector2f& playerPosition,
        std::vector<Projectile>& hostileProjectiles);
  void draw(sf::RenderTarget& target) const;
  void takeDamage(int amount) noexcept;

  [[nodiscard]] bool isActive() const noexcept { return active_; }
  [[nodiscard]] bool isDefeated() const noexcept { return defeated_; }
  [[nodiscard]] const sf::Vector2f& getPosition() const noexcept { return position_; }
  [[nodiscard]] float getRadius() const noexcept { return 58.0F; }
  [[nodiscard]] int getHealth() const noexcept { return health_; }
  [[nodiscard]] int getMaxHealth() const noexcept { return maxHealth_; }

private:
  void fireAtPlayer(const sf::Vector2f& playerPosition,
           std::vector<Projectile>& hostileProjectiles) const;

  sf::Vector2f position_{640.0F, 115.0F};
  sf::Vector2f spawnPosition_{640.0F, 115.0F};
  int health_{260};
  int maxHealth_{260};
  bool active_{false};
  bool defeated_{false};
  float elapsed_{0.0F};
  float attackTimer_{1.0F};
};

}  // namespace space_survivor
