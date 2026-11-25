#include "Projectile.hpp"

#include "Math.hpp"
#include "Types.hpp"

#include <algorithm>

namespace space_survivor {
namespace {
constexpr float MaxLifetime = 5.0F;
constexpr float HorizontalMargin = 40.0F;
constexpr float TopMargin = 60.0F;
constexpr float BottomMargin = 60.0F;
}  // namespace

Projectile::Projectile(const sf::Vector2f& position, const sf::Vector2f& velocity,
                       float radius, int damage, bool hostile, const sf::Color& color)
  : position_(position), velocity_(velocity), radius_(std::max(0.1F, radius)),
   damage_(std::max(0, damage)), hostile_(hostile), color_(color) {}

// AUDIT-007: Invalid frame deltas cannot extend projectile lifetime or reverse movement.
void Projectile::update(float deltaTime) noexcept {
  const float dt = math::safeDelta(deltaTime);
  position_ += velocity_ * dt;
  lifetime_ += dt;
  const float maxX = static_cast<float>(config::WindowWidth) + HorizontalMargin;
  const float maxY = static_cast<float>(config::WindowHeight) + BottomMargin;
  if (lifetime_ >= MaxLifetime || position_.x < -HorizontalMargin ||
    position_.x > maxX || position_.y < -TopMargin || position_.y > maxY) {
    alive_ = false;
  }
}

void Projectile::draw(sf::RenderTarget& target) const {
  if (!alive_) {
    return;
  }

  sf::CircleShape glow(radius_ * 2.4F);
  glow.setOrigin(glow.getRadius(), glow.getRadius());
  glow.setPosition(position_);
  sf::Color glowColor = color_;
  glowColor.a = 65;
  glow.setFillColor(glowColor);
  target.draw(glow);

  sf::RectangleShape beam({radius_ * 1.4F, radius_ * 4.2F});
  beam.setOrigin(beam.getSize().x / 2.0F, beam.getSize().y / 2.0F);
  beam.setPosition(position_);
  beam.setFillColor(color_);
  target.draw(beam);

  sf::CircleShape core(radius_ * 0.65F);
  core.setOrigin(core.getRadius(), core.getRadius());
  core.setPosition(position_);
  core.setFillColor(sf::Color::White);
  target.draw(core);
}

}  // namespace space_survivor
