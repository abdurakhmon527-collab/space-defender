#include "Boss.hpp"

#include "Math.hpp"
#include "Types.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace space_survivor {
namespace {
constexpr float HorizontalTravel = 310.0F;
constexpr float HorizontalRate = 0.72F;
constexpr float VerticalTravel = 18.0F;
constexpr float VerticalRate = 1.8F;
constexpr float InitialAttackDelay = 1.0F;
constexpr float AttackInterval = 1.12F;
constexpr float ProjectileSpeed = 265.0F;
constexpr float ProjectileRadius = 6.0F;
constexpr int ProjectileDamage = 1;
constexpr std::array<float, 3U> AttackAngles{-0.24F, 0.0F, 0.24F};
}  // namespace

void Boss::spawn(const sf::Vector2f& position) noexcept {
  spawnPosition_ = position;
  position_ = position;
  health_ = maxHealth_;
  active_ = true;
  defeated_ = false;
  elapsed_ = 0.0F;
  attackTimer_ = InitialAttackDelay;
}

void Boss::update(float deltaTime, const sf::Vector2f& playerPosition,
         std::vector<Projectile>& hostileProjectiles) {
  if (!active_) {
    return;
  }

  const float dt = math::safeDelta(deltaTime);
  elapsed_ += dt;
  position_.x = spawnPosition_.x + std::sin(elapsed_ * HorizontalRate) * HorizontalTravel;
  position_.y = spawnPosition_.y + std::sin(elapsed_ * VerticalRate) * VerticalTravel;
  attackTimer_ -= dt;

  if (attackTimer_ <= 0.0F) {
    attackTimer_ += AttackInterval;
    fireAtPlayer(playerPosition, hostileProjectiles);
  }
}

void Boss::fireAtPlayer(const sf::Vector2f& playerPosition,
            std::vector<Projectile>& hostileProjectiles) const {
  // AUDIT-010: Keep the hostile projectile collection bounded during a long boss fight.
  if (hostileProjectiles.size() + AttackAngles.size() > config::MaxHostileProjectiles) {
    return;
  }

  const sf::Vector2f baseDirection = math::normalized(playerPosition - position_);
  for (const float angle : AttackAngles) {
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    const sf::Vector2f direction{
      baseDirection.x * cosine - baseDirection.y * sine,
      baseDirection.x * sine + baseDirection.y * cosine
    };
    hostileProjectiles.emplace_back(position_ + direction * 48.0F,
                    direction * ProjectileSpeed,
                    ProjectileRadius, ProjectileDamage, true,
                    sf::Color(255, 65, 135));
  }
}

void Boss::draw(sf::RenderTarget& target) const {
  if (!active_) {
    return;
  }

  sf::CircleShape aura(88.0F);
  aura.setOrigin(88.0F, 88.0F);
  aura.setPosition(position_);
  aura.setFillColor(sf::Color(255, 35, 130, 27));
  target.draw(aura);

  sf::ConvexShape wings(6U);
  wings.setPoint(0U, {-22.0F, -12.0F});
  wings.setPoint(1U, {-95.0F, -42.0F});
  wings.setPoint(2U, {-72.0F, 30.0F});
  wings.setPoint(3U, {0.0F, 56.0F});
  wings.setPoint(4U, {72.0F, 30.0F});
  wings.setPoint(5U, {95.0F, -42.0F});
  wings.setPosition(position_);
  wings.setFillColor(sf::Color(110, 24, 90));
  wings.setOutlineThickness(3.0F);
  wings.setOutlineColor(sf::Color(255, 80, 180));
  target.draw(wings);

  sf::ConvexShape body(5U);
  body.setPoint(0U, {0.0F, -63.0F});
  body.setPoint(1U, {49.0F, -7.0F});
  body.setPoint(2U, {30.0F, 45.0F});
  body.setPoint(3U, {-30.0F, 45.0F});
  body.setPoint(4U, {-49.0F, -7.0F});
  body.setPosition(position_);
  body.setFillColor(sf::Color(52, 22, 68));
  body.setOutlineThickness(3.0F);
  body.setOutlineColor(sf::Color(255, 110, 200));
  target.draw(body);

  sf::CircleShape core(18.0F);
  core.setOrigin(18.0F, 18.0F);
  core.setPosition(position_);
  core.setFillColor(sf::Color(255, 75, 155));
  core.setOutlineThickness(4.0F);
  core.setOutlineColor(sf::Color(255, 200, 245));
  target.draw(core);
}

void Boss::takeDamage(int amount) noexcept {
  if (!active_ || amount <= 0) {
    return;
  }

  // AUDIT-002: Clamp before subtraction; a large hit must not wrap the health counter.
  health_ = amount >= health_ ? 0 : health_ - amount;
  if (health_ == 0) {
    active_ = false;
    defeated_ = true;
  }
}

}  // namespace space_survivor
