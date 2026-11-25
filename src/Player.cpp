#include "Player.hpp"

#include "Math.hpp"

#include <algorithm>

namespace space_survivor {
namespace {
constexpr float NormalShotCooldown = 0.25F;
constexpr float RapidShotCooldown = 0.105F;
constexpr float InvulnerabilityDuration = config::PlayerInvulnerabilityDuration;
constexpr float ProjectileSpeed = 650.0F;
constexpr float ProjectileRadius = 3.8F;
constexpr float ProjectilePairOffset = 9.0F;
constexpr float ProjectileSpawnYOffset = 17.0F;
constexpr float HorizontalBoundaryPadding = 8.0F;
constexpr float BottomBoundaryPadding = 12.0F;
constexpr float DamageBlinkRate = 14.0F;
constexpr float ShipMinY = 90.0F;
}  // namespace

void Player::reset(const sf::Vector2f& startPosition) noexcept {
  position_ = startPosition;
  velocity_ = {0.0F, 0.0F};
  health_ = maxHealth_;
  shootCooldown_ = 0.0F;
  invulnerabilityTimer_ = 0.0F;
  rapidFireTimer_ = 0.0F;
  shieldTimer_ = 0.0F;
}

void Player::update(float deltaTime) {
  sf::Vector2f input{0.0F, 0.0F};
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
    input.y -= 1.0F;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
    input.y += 1.0F;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
    input.x -= 1.0F;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    input.x += 1.0F;
  }
  update(deltaTime, input);
}

// AUDIT-005: Explicit input keeps movement deterministic in tests and replay tools.
void Player::update(float deltaTime, const sf::Vector2f& inputDirection) noexcept {
  const float dt = math::safeDelta(deltaTime);
  velocity_ = math::normalized(inputDirection) * moveSpeed_;
  position_ += velocity_ * dt;
  position_.x = math::clamp(position_.x, radius_ + HorizontalBoundaryPadding,
               static_cast<float>(config::WindowWidth) - radius_ - HorizontalBoundaryPadding);
  position_.y = math::clamp(position_.y, ShipMinY,
               static_cast<float>(config::WindowHeight) - radius_ - BottomBoundaryPadding);

  shootCooldown_ = std::max(0.0F, shootCooldown_ - dt);
  invulnerabilityTimer_ = std::max(0.0F, invulnerabilityTimer_ - dt);
  rapidFireTimer_ = std::max(0.0F, rapidFireTimer_ - dt);
  shieldTimer_ = std::max(0.0F, shieldTimer_ - dt);
}

void Player::shoot(std::vector<Projectile>& projectiles, bool rapidFireEnabled) {
  if (shootCooldown_ > 0.0F) {
    return;
  }

  const bool rapid = rapidFireEnabled || hasRapidFire();
  shootCooldown_ = rapid ? RapidShotCooldown : NormalShotCooldown;
  projectiles.emplace_back(
    sf::Vector2f{position_.x - ProjectilePairOffset, position_.y - ProjectileSpawnYOffset},
    sf::Vector2f{0.0F, -ProjectileSpeed}, ProjectileRadius, 1, false,
    sf::Color(60, 245, 255));
  projectiles.emplace_back(
    sf::Vector2f{position_.x + ProjectilePairOffset, position_.y - ProjectileSpawnYOffset},
    sf::Vector2f{0.0F, -ProjectileSpeed}, ProjectileRadius, 1, false,
    sf::Color(60, 245, 255));
}

void Player::draw(sf::RenderTarget& target) const {
  if (invulnerabilityTimer_ > 0.0F && !hasShield() &&
    static_cast<int>(invulnerabilityTimer_ * DamageBlinkRate) % 2 == 0) {
    return;
  }

  if (hasShield()) {
    sf::CircleShape shield(radius_ + 13.0F);
    shield.setOrigin(shield.getRadius(), shield.getRadius());
    shield.setPosition(position_);
    shield.setFillColor(sf::Color(30, 180, 255, 24));
    shield.setOutlineThickness(2.0F);
    shield.setOutlineColor(sf::Color(70, 190, 255, 190));
    target.draw(shield);
  }

  sf::ConvexShape flame(3U);
  flame.setPoint(0U, {0.0F, 0.0F});
  flame.setPoint(1U, {-7.0F, 22.0F});
  flame.setPoint(2U, {7.0F, 22.0F});
  flame.setPosition(position_.x, position_.y + 8.0F);
  flame.setFillColor(sf::Color(255, 65, 210, 210));
  target.draw(flame);

  sf::ConvexShape ship(5U);
  ship.setPoint(0U, {0.0F, -29.0F});
  ship.setPoint(1U, {17.0F, 16.0F});
  ship.setPoint(2U, {5.0F, 10.0F});
  ship.setPoint(3U, {0.0F, 15.0F});
  ship.setPoint(4U, {-17.0F, 16.0F});
  ship.setPosition(position_);
  ship.setFillColor(sf::Color(32, 205, 245));
  ship.setOutlineThickness(2.0F);
  ship.setOutlineColor(sf::Color(190, 250, 255));
  target.draw(ship);

  sf::ConvexShape cockpit(3U);
  cockpit.setPoint(0U, {0.0F, -16.0F});
  cockpit.setPoint(1U, {6.0F, 3.0F});
  cockpit.setPoint(2U, {-6.0F, 3.0F});
  cockpit.setPosition(position_);
  cockpit.setFillColor(sf::Color(180, 250, 255));
  target.draw(cockpit);
}

bool Player::takeDamage(int amount) noexcept {
  if (amount <= 0 || health_ <= 0 || isInvulnerable()) {
    return false;
  }

  health_ = amount >= health_ ? 0 : health_ - amount;
  invulnerabilityTimer_ = InvulnerabilityDuration;
  return true;
}

void Player::collect(PowerUpType type) noexcept {
  switch (type) {
    case PowerUpType::Health:
      health_ = std::min(maxHealth_, health_ + 1);
      break;
    case PowerUpType::RapidFire:
      rapidFireTimer_ = config::RapidFireDuration;
      break;
    case PowerUpType::Shield:
      shieldTimer_ = config::ShieldDuration;
      break;
  }
}

}  // namespace space_survivor
