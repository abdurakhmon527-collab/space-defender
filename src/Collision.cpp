#include "Collision.hpp"

#include "Math.hpp"

#include <algorithm>
#include <cmath>

namespace space_survivor {

bool Collision::circles(const sf::Vector2f& first, float firstRadius,
            const sf::Vector2f& second, float secondRadius) noexcept {
  if (!std::isfinite(firstRadius) || !std::isfinite(secondRadius)) {
    return false;
  }

  // AUDIT-011: Squared distance avoids a square root for every collision pair.
  const float radius = std::max(0.0F, firstRadius) + std::max(0.0F, secondRadius);
  return math::distanceSquared(first, second) <= radius * radius;
}

bool Collision::circleRectangle(const sf::Vector2f& center, float radius,
                const sf::FloatRect& rectangle) noexcept {
  if (!std::isfinite(radius) || !std::isfinite(rectangle.left) ||
    !std::isfinite(rectangle.top) || !std::isfinite(rectangle.width) ||
    !std::isfinite(rectangle.height)) {
    return false;
  }

  const float left = std::min(rectangle.left, rectangle.left + rectangle.width);
  const float right = std::max(rectangle.left, rectangle.left + rectangle.width);
  const float top = std::min(rectangle.top, rectangle.top + rectangle.height);
  const float bottom = std::max(rectangle.top, rectangle.top + rectangle.height);
  const sf::Vector2f closest{
    std::clamp(center.x, left, right),
    std::clamp(center.y, top, bottom)
  };
  const float safeRadius = std::max(0.0F, radius);
  return math::distanceSquared(center, closest) <= safeRadius * safeRadius;
}

}  // namespace space_survivor
