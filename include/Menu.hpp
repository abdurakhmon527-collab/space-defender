#pragma once

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

namespace space_survivor {

class Menu {
public:
  void setContent(const std::string& title, const std::string& subtitle,
          const std::vector<std::string>& items);
  [[nodiscard]] int handleEvent(const sf::Event& event,
                 const sf::Vector2f& mousePosition) noexcept;
  void draw(sf::RenderTarget& target, const sf::Font& font, bool hasFont,
       float animationTime) const;

private:
  struct Button {
    std::string label;
    sf::FloatRect bounds;
  };

  void drawHeading(sf::RenderTarget& target, const sf::Font& font, bool hasFont) const;
  void drawButtons(sf::RenderTarget& target, const sf::Font& font, bool hasFont) const;
  void drawFooter(sf::RenderTarget& target, const sf::Font& font, bool hasFont) const;

  std::string title_;
  std::string subtitle_;
  std::vector<Button> buttons_;
  int selectedIndex_{0};
};

}  // namespace space_survivor
