#include "PowerUp.hpp"

#include "Math.hpp"

#include <cmath>

namespace space_survivor {
namespace {
constexpr float FallSpeed = 92.0F;
constexpr float SwaySpeed = 2.5F;
constexpr float SwayDistance = 24.0F;
constexpr float OffscreenMargin = 50.0F;

void drawHealthIcon(sf::RenderTarget& target, const sf::Vector2f& position,
                    float radius, sf::Color color) {
  sf::RectangleShape vertical({4.0F, radius * 1.05F});
  vertical.setOrigin(2.0F, vertical.getSize().y / 2.0F);
  vertical.setPosition(position);
  vertical.setFillColor(color);
  target.draw(vertical);

  sf::RectangleShape horizontal({radius * 1.05F, 4.0F});
  horizontal.setOrigin(horizontal.getSize().x / 2.0F, 2.0F);
  horizontal.setPosition(position);
  horizontal.setFillColor(color);
  target.draw(horizontal);
}

void drawRapidFireIcon(sf::RenderTarget& target, const sf::Vector2f& position,
                       sf::Color color) {
  sf::ConvexShape bolt(6U);
  bolt.setPoint(0U, {2.0F, -12.0F});
  bolt.setPoint(1U, {-8.0F, 1.0F});
  bolt.setPoint(2U, {-1.0F, 1.0F});
  bolt.setPoint(3U, {-4.0F, 12.0F});
  bolt.setPoint(4U, {9.0F, -3.0F});
  bolt.setPoint(5U, {2.0F, -3.0F});
  bolt.setPosition(position);
  bolt.setFillColor(color);
  target.draw(bolt);
}

void drawShieldIcon(sf::RenderTarget& target, const sf::Vector2f& position,
                    sf::Color color) {
  sf::ConvexShape shield(5U);
  shield.setPoint(0U, {0.0F, -12.0F});
  shield.setPoint(1U, {10.0F, -7.0F});
  shield.setPoint(2U, {7.0F, 6.0F});
  shield.setPoint(3U, {0.0F, 12.0F});
  shield.setPoint(4U, {-7.0F, 6.0F});
  shield.setPosition(position);
  shield.setFillColor(color);
  target.draw(shield);
}
}  // namespace

PowerUp::PowerUp(PowerUpType type, const sf::Vector2f& position)
  : type_(type), position_(position) {}

void PowerUp::update(float deltaTime) noexcept {
  if (!alive_) {
    return;
  }

  const float dt = math::safeDelta(deltaTime);
  age_ += dt;
  position_.y += FallSpeed * dt;
  position_.x += std::sin(age_ * SwaySpeed) * SwayDistance * dt;
  if (age_ >= config::PowerUpLifetime ||
    position_.y > static_cast<float>(config::WindowHeight) + OffscreenMargin) {
    alive_ = false;
  }
}

void PowerUp::draw(sf::RenderTarget& target) const {
  if (!alive_) {
    return;
  }

  sf::Color color(80, 255, 140);
  if (type_ == PowerUpType::RapidFire) {
    color = sf::Color(255, 80, 225);
  } else if (type_ == PowerUpType::Shield) {
    color = sf::Color(65, 190, 255);
  }

  const float pulse = 1.0F + std::sin(age_ * 6.0F) * 0.08F;
  sf::CircleShape aura(radius_ * 1.7F * pulse);
  aura.setOrigin(aura.getRadius(), aura.getRadius());
  aura.setPosition(position_);
  sf::Color auraColor = color;
  auraColor.a = 48;
  aura.setFillColor(auraColor);
  target.draw(aura);

  sf::CircleShape token(radius_ * pulse);
  token.setOrigin(token.getRadius(), token.getRadius());
  token.setPosition(position_);
  token.setFillColor(sf::Color(15, 22, 42));
  token.setOutlineThickness(3.0F);
  token.setOutlineColor(color);
  target.draw(token);

  switch (type_) {
    case PowerUpType::Health:
      drawHealthIcon(target, position_, radius_, color);
      break;
    case PowerUpType::RapidFire:
      drawRapidFireIcon(target, position_, color);
      break;
    case PowerUpType::Shield:
      drawShieldIcon(target, position_, color);
      break;
  }
}

}  // namespace space_survivor
