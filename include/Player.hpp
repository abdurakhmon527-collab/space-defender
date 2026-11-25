#pragma once

#include "Projectile.hpp"
#include "Types.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

namespace space_survivor {

class Player {
public:
  Player() = default;

  void reset(const sf::Vector2f& startPosition) noexcept;
  void update(float deltaTime);
  void update(float deltaTime, const sf::Vector2f& inputDirection) noexcept;
  void shoot(std::vector<Projectile>& projectiles, bool rapidFireEnabled);
  void draw(sf::RenderTarget& target) const;
  [[nodiscard]] bool takeDamage(int amount = 1) noexcept;
  void collect(PowerUpType type) noexcept;

  // AUDIT-017: Read-only accessors make their result and side-effect contract explicit.
  [[nodiscard]] const sf::Vector2f& getPosition() const noexcept { return position_; }
  [[nodiscard]] float getRadius() const noexcept { return radius_; }
  [[nodiscard]] int getHealth() const noexcept { return health_; }
  [[nodiscard]] int getMaxHealth() const noexcept { return maxHealth_; }
  [[nodiscard]] bool hasShield() const noexcept { return shieldTimer_ > 0.0F; }
  [[nodiscard]] bool isInvulnerable() const noexcept {
    return invulnerabilityTimer_ > 0.0F || hasShield();
  }
  [[nodiscard]] bool hasRapidFire() const noexcept { return rapidFireTimer_ > 0.0F; }
  [[nodiscard]] float getRapidFireTime() const noexcept { return rapidFireTimer_; }
  [[nodiscard]] float getShieldTime() const noexcept { return shieldTimer_; }

private:
  sf::Vector2f position_{640.0F, 590.0F};
  sf::Vector2f velocity_{0.0F, 0.0F};
  float radius_{config::PlayerCollisionRadius};
  float moveSpeed_{config::PlayerMoveSpeed};
  int health_{config::StartingHealth};
  int maxHealth_{config::StartingHealth};
  float shootCooldown_{0.0F};
  float invulnerabilityTimer_{0.0F};
  float rapidFireTimer_{0.0F};
  float shieldTimer_{0.0F};
};

}  // namespace space_survivor
