#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace space_survivor {

class Collision {
public:
  [[nodiscard]] static bool circles(const sf::Vector2f& first, float firstRadius,
                   const sf::Vector2f& second, float secondRadius) noexcept;
  [[nodiscard]] static bool circleRectangle(const sf::Vector2f& center, float radius,
                       const sf::FloatRect& rectangle) noexcept;
};

}  // namespace space_survivor
