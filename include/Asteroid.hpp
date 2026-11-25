#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

namespace space_survivor {

class Asteroid {
public:
  Asteroid(const sf::Vector2f& position, float radius,
             const sf::Vector2f& velocity, float rotationSpeed);

  void update(float deltaTime) noexcept;
  void draw(sf::RenderTarget& target) const;
  void takeDamage(int amount) noexcept;
  void destroy() noexcept { alive_ = false; }

  [[nodiscard]] const sf::Vector2f& getPosition() const noexcept { return position_; }
  [[nodiscard]] float getRadius() const noexcept { return radius_; }
  [[nodiscard]] int getHealth() const noexcept { return health_; }
  [[nodiscard]] bool isAlive() const noexcept { return alive_; }

private:
  sf::Vector2f position_;
  sf::Vector2f velocity_;
  float radius_;
  float rotation_{0.0F};
  float rotationSpeed_;
  int health_;
  bool alive_{true};
  std::vector<sf::Vector2f> vertices_;
};

}  // namespace space_survivor
