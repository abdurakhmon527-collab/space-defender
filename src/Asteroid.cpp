#include "Asteroid.hpp"

#include "Math.hpp"

#include <algorithm>
#include <cmath>
#include <random>

namespace space_survivor {
namespace {
constexpr std::size_t VertexCount = 9U;
constexpr float MinRadius = 8.0F;
constexpr float OffscreenMargin = 100.0F;
constexpr float HeavyAsteroidRadiusThreshold = 30.0F;
constexpr float MinVertexRadiusScale = 0.78F;
constexpr float MaxVertexRadiusScale = 1.13F;

float randomRadiusScale() {
  static std::mt19937 engine{std::random_device{}()};
  static std::uniform_real_distribution<float> distribution(
    MinVertexRadiusScale, MaxVertexRadiusScale);
  return distribution(engine);
}
}  // namespace

Asteroid::Asteroid(const sf::Vector2f& position, float radius,
                   const sf::Vector2f& velocity, float rotationSpeed)
  : position_(position), velocity_(velocity), radius_(std::max(MinRadius, radius)),
   rotationSpeed_(rotationSpeed), health_(radius_ > HeavyAsteroidRadiusThreshold ? 3 : 2) {
  vertices_.reserve(VertexCount);
  for (std::size_t idx = 0; idx < VertexCount; ++idx) {
    const float angle = (2.0F * config::Pi * static_cast<float>(idx)) /
              static_cast<float>(VertexCount);
    const float vertexRadius = radius_ * randomRadiusScale();
    vertices_.push_back({std::cos(angle) * vertexRadius,
                             std::sin(angle) * vertexRadius});
  }
}

void Asteroid::update(float deltaTime) noexcept {
  if (!alive_) {
    return;
  }

  const float dt = math::safeDelta(deltaTime);
  position_ += velocity_ * dt;
  rotation_ += rotationSpeed_ * dt;
  const float maxX = static_cast<float>(config::WindowWidth) + OffscreenMargin;
  const float maxY = static_cast<float>(config::WindowHeight) + OffscreenMargin;
  if (position_.y > maxY || position_.x < -OffscreenMargin || position_.x > maxX) {
    alive_ = false;
  }
}

void Asteroid::draw(sf::RenderTarget& target) const {
  if (!alive_) {
    return;
  }

  sf::CircleShape aura(radius_ * 1.45F);
  aura.setOrigin(aura.getRadius(), aura.getRadius());
  aura.setPosition(position_);
  aura.setFillColor(sf::Color(130, 95, 180, 22));
  target.draw(aura);

  sf::ConvexShape rock(vertices_.size());
  for (std::size_t idx = 0; idx < vertices_.size(); ++idx) {
    rock.setPoint(idx, vertices_[idx]);
  }
  rock.setPosition(position_);
  rock.setRotation(rotation_);
  rock.setFillColor(sf::Color(76, 77, 104));
  rock.setOutlineThickness(2.0F);
  rock.setOutlineColor(sf::Color(165, 145, 205));
  target.draw(rock);

  sf::CircleShape crater(radius_ * 0.19F);
  crater.setOrigin(crater.getRadius(), crater.getRadius());
  crater.setPosition(position_.x - radius_ * 0.24F, position_.y + radius_ * 0.08F);
  crater.setFillColor(sf::Color(45, 45, 72));
  target.draw(crater);
}

void Asteroid::takeDamage(int amount) noexcept {
  if (!alive_ || amount <= 0) {
    return;
  }

  // AUDIT-002: Compare before subtracting to avoid signed integer underflow.
  health_ = amount >= health_ ? 0 : health_ - amount;
  alive_ = health_ > 0;
}

}  // namespace space_survivor
