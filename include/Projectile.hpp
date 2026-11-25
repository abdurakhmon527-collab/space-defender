#pragma once

#include <SFML/Graphics.hpp>

namespace space_survivor {

class Projectile {
public:
  Projectile(const sf::Vector2f& position, const sf::Vector2f& velocity,
               float radius, int damage, bool hostile,
               const sf::Color& color = sf::Color::Cyan);

  void update(float deltaTime) noexcept;
  void draw(sf::RenderTarget& target) const;
  void destroy() noexcept { alive_ = false; }

  [[nodiscard]] const sf::Vector2f& getPosition() const noexcept { return position_; }
  [[nodiscard]] float getRadius() const noexcept { return radius_; }
  [[nodiscard]] int getDamage() const noexcept { return damage_; }
  [[nodiscard]] bool isHostile() const noexcept { return hostile_; }
  [[nodiscard]] bool isAlive() const noexcept { return alive_; }

private:
  sf::Vector2f position_;
  sf::Vector2f velocity_;
  float radius_;
  int damage_;
  bool hostile_;
  bool alive_{true};
  float lifetime_{0.0F};
  sf::Color color_;
};

}  // namespace space_survivor
