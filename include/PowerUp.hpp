#pragma once

#include "Types.hpp"

#include <SFML/Graphics.hpp>

namespace space_survivor {

class PowerUp {
public:
  PowerUp(PowerUpType type, const sf::Vector2f& position);

  void update(float deltaTime) noexcept;
  void draw(sf::RenderTarget& target) const;
  void collect() noexcept { alive_ = false; }

  [[nodiscard]] const sf::Vector2f& getPosition() const noexcept { return position_; }
  [[nodiscard]] float getRadius() const noexcept { return radius_; }
  [[nodiscard]] PowerUpType getType() const noexcept { return type_; }
  [[nodiscard]] float getAge() const noexcept { return age_; }
  [[nodiscard]] bool isAlive() const noexcept { return alive_; }

private:
  PowerUpType type_;
  sf::Vector2f position_;
  float radius_{15.0F};
  float age_{0.0F};
  bool alive_{true};
};

}  // namespace space_survivor
