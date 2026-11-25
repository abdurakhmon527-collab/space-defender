#include "Enemy.hpp"

#include "Math.hpp"

#include <cmath>

namespace space_survivor {
namespace {
constexpr float OffscreenMargin = 90.0F;
constexpr float FastSwayFrequency = 6.0F;
constexpr float FastSwayAmplitude = 36.0F;
struct EnemyStats {
  float radius;
  float speed;
  int health;
  int contactDamage;
  int scoreReward;
};
constexpr EnemyStats BasicStats{19.0F, 122.0F, 2, 1, 100};
constexpr EnemyStats FastStats{15.0F, 205.0F, 1, 1, 150};
constexpr EnemyStats HeavyStats{27.0F, 78.0F, 6, 2, 300};
}  // namespace

Enemy::Enemy(EnemyType type, const sf::Vector2f& position)
  : type_(type), position_(position) {
  switch (type_) {
    case EnemyType::Basic:
      radius_ = BasicStats.radius;
      speed_ = BasicStats.speed;
      health_ = BasicStats.health;
      contactDamage_ = BasicStats.contactDamage;
      scoreReward_ = BasicStats.scoreReward;
      break;
    case EnemyType::Fast:
      radius_ = FastStats.radius;
      speed_ = FastStats.speed;
      health_ = FastStats.health;
      contactDamage_ = FastStats.contactDamage;
      scoreReward_ = FastStats.scoreReward;
      break;
    case EnemyType::Heavy:
      radius_ = HeavyStats.radius;
      speed_ = HeavyStats.speed;
      health_ = HeavyStats.health;
      contactDamage_ = HeavyStats.contactDamage;
      scoreReward_ = HeavyStats.scoreReward;
      break;
  }
}

void Enemy::update(float deltaTime, const sf::Vector2f& playerPosition) noexcept {
  if (!alive_) {
    return;
  }

  const float dt = math::safeDelta(deltaTime);
  animationTime_ += dt;
  const sf::Vector2f direction = math::normalized(playerPosition - position_);
  const float sway = type_ == EnemyType::Fast
                           ? std::sin(animationTime_ * FastSwayFrequency) * FastSwayAmplitude
                           : 0.0F;
  position_ += sf::Vector2f{direction.x * speed_ + sway, direction.y * speed_} * dt;

  // AUDIT-008: Retire enemies that leave the play area so the spawn cap can recover.
  if (position_.y > static_cast<float>(config::WindowHeight) + OffscreenMargin ||
    position_.x < -OffscreenMargin ||
    position_.x > static_cast<float>(config::WindowWidth) + OffscreenMargin) {
    alive_ = false;
  }
}

void Enemy::draw(sf::RenderTarget& target) const {
  if (!alive_) {
    return;
  }

  sf::Color color(255, 70, 105);
  if (type_ == EnemyType::Fast) {
    color = sf::Color(255, 70, 205);
  } else if (type_ == EnemyType::Heavy) {
    color = sf::Color(255, 125, 50);
  }

  sf::CircleShape aura(radius_ * 1.65F);
  aura.setOrigin(aura.getRadius(), aura.getRadius());
  aura.setPosition(position_);
  sf::Color auraColor = color;
  auraColor.a = 34;
  aura.setFillColor(auraColor);
  target.draw(aura);

  sf::ConvexShape hull(6U);
  hull.setPoint(0U, {0.0F, radius_});
  hull.setPoint(1U, {-radius_ * 0.95F, -radius_ * 0.1F});
  hull.setPoint(2U, {-radius_ * 0.52F, -radius_ * 0.76F});
  hull.setPoint(3U, {radius_ * 0.52F, -radius_ * 0.76F});
  hull.setPoint(4U, {radius_ * 0.95F, -radius_ * 0.1F});
  hull.setPoint(5U, {0.0F, radius_ * 0.5F});
  hull.setPosition(position_);
  hull.setFillColor(color);
  hull.setOutlineThickness(2.0F);
  hull.setOutlineColor(sf::Color(255, 220, 240));
  target.draw(hull);

  sf::CircleShape core(radius_ * 0.27F);
  core.setOrigin(core.getRadius(), core.getRadius());
  core.setPosition(position_);
  core.setFillColor(sf::Color(35, 12, 45));
  target.draw(core);

  if (type_ == EnemyType::Heavy) {
    sf::RectangleShape healthBar({radius_ * 1.5F, 3.0F});
    healthBar.setOrigin(healthBar.getSize().x / 2.0F, healthBar.getSize().y / 2.0F);
    healthBar.setPosition(position_.x, position_.y - radius_ - 9.0F);
    healthBar.setFillColor(sf::Color(255, 205, 90));
    target.draw(healthBar);
  }
}

void Enemy::takeDamage(int amount) noexcept {
  if (!alive_ || amount <= 0) {
    return;
  }

  // AUDIT-002: Clamp before subtraction; damage may exceed the current hit points.
  health_ = amount >= health_ ? 0 : health_ - amount;
  alive_ = health_ > 0;
}

}  // namespace space_survivor
