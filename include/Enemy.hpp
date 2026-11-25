#pragma once

#include "Types.hpp"

#include <SFML/Graphics.hpp>

namespace space_survivor {

class Enemy {
public:
  Enemy(EnemyType type, const sf::Vector2f& position);

  void update(float deltaTime, const sf::Vector2f& playerPosition) noexcept;
  void draw(sf::RenderTarget& target) const;
  void takeDamage(int amount) noexcept;
  void destroy() noexcept { alive_ = false; }

  [[nodiscard]] const sf::Vector2f& getPosition() const noexcept { return position_; }
  [[nodiscard]] float getRadius() const noexcept { return radius_; }
  [[nodiscard]] int getDamage() const noexcept { return contactDamage_; }
  [[nodiscard]] int getScoreReward() const noexcept { return scoreReward_; }
  [[nodiscard]] int getHealth() const noexcept { return health_; }
  [[nodiscard]] EnemyType getType() const noexcept { return type_; }
  [[nodiscard]] bool isAlive() const noexcept { return alive_; }

private:
  EnemyType type_;
  sf::Vector2f position_;
  float radius_{20.0F};
  float speed_{125.0F};
  int health_{1};
  int contactDamage_{1};
  int scoreReward_{100};
  bool alive_{true};
  float animationTime_{0.0F};
};

}  // namespace space_survivor
