#include "Menu.hpp"

#include "Types.hpp"

#include <algorithm>
#include <cstddef>

namespace space_survivor {
namespace {
constexpr float ButtonWidth = 340.0F;
constexpr float ButtonHeight = 48.0F;
constexpr float ButtonGap = 12.0F;
constexpr float MenuCenterX = static_cast<float>(config::WindowWidth) / 2.0F;
}  // namespace

void Menu::setContent(const std::string& title, const std::string& subtitle,
           const std::vector<std::string>& items) {
  title_ = title;
  subtitle_ = subtitle;
  buttons_.clear();
  selectedIndex_ = 0;
  buttons_.reserve(items.size());

  const float count = static_cast<float>(items.size());
  const float gapCount = items.empty() ? 0.0F : count - 1.0F;
  const float totalHeight = count * ButtonHeight + gapCount * ButtonGap;
  const float startY = std::max(270.0F, 360.0F - totalHeight * 0.38F);
  for (std::size_t idx = 0; idx < items.size(); ++idx) {
    const float y = startY + static_cast<float>(idx) * (ButtonHeight + ButtonGap);
    buttons_.push_back({items[idx],
              {MenuCenterX - ButtonWidth / 2.0F, y,
                             ButtonWidth, ButtonHeight}});
  }
}

int Menu::handleEvent(const sf::Event& event, const sf::Vector2f& mousePosition) noexcept {
  if (buttons_.empty()) {
    return -1;
  }

  if (event.type == sf::Event::MouseMoved) {
    for (std::size_t idx = 0; idx < buttons_.size(); ++idx) {
      if (buttons_[idx].bounds.contains(mousePosition)) {
        selectedIndex_ = static_cast<int>(idx);
        break;
      }
    }
  }

  if (event.type == sf::Event::KeyPressed) {
    const int count = static_cast<int>(buttons_.size());
    if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
      selectedIndex_ = (selectedIndex_ - 1 + count) % count;
    } else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
      selectedIndex_ = (selectedIndex_ + 1) % count;
    } else if (event.key.code == sf::Keyboard::Enter ||
                   event.key.code == sf::Keyboard::Space) {
      return selectedIndex_;
    }
  }

  if (event.type == sf::Event::MouseButtonPressed &&
    event.mouseButton.button == sf::Mouse::Left) {
    for (std::size_t idx = 0; idx < buttons_.size(); ++idx) {
      if (buttons_[idx].bounds.contains(mousePosition)) {
        selectedIndex_ = static_cast<int>(idx);
        return selectedIndex_;
      }
    }
  }
  return -1;
}

void Menu::draw(sf::RenderTarget& target, const sf::Font& font, bool hasFont,
        float animationTime) const {
  sf::RectangleShape overlay({static_cast<float>(config::WindowWidth),
                static_cast<float>(config::WindowHeight)});
  overlay.setFillColor(sf::Color(3, 5, 18, 218));
  target.draw(overlay);

  sf::CircleShape outerRing(205.0F);
  outerRing.setOrigin(205.0F, 205.0F);
  outerRing.setPosition(MenuCenterX, 255.0F);
  outerRing.setFillColor(sf::Color::Transparent);
  outerRing.setOutlineThickness(1.5F);
  outerRing.setOutlineColor(sf::Color(45, 185, 255, 48));
  outerRing.setRotation(animationTime * 7.0F);
  target.draw(outerRing);

  sf::CircleShape innerRing(150.0F);
  innerRing.setOrigin(150.0F, 150.0F);
  innerRing.setPosition(MenuCenterX, 255.0F);
  innerRing.setFillColor(sf::Color::Transparent);
  innerRing.setOutlineThickness(1.0F);
  innerRing.setOutlineColor(sf::Color(205, 55, 220, 55));
  innerRing.setRotation(-animationTime * 11.0F);
  target.draw(innerRing);

  drawHeading(target, font, hasFont);
  drawButtons(target, font, hasFont);
  drawFooter(target, font, hasFont);
}

void Menu::drawHeading(sf::RenderTarget& target, const sf::Font& font, bool hasFont) const {
  if (!hasFont) {
    return;
  }

  sf::Text title(title_, font, 48U);
  title.setStyle(sf::Text::Bold);
  title.setLetterSpacing(1.3F);
  title.setFillColor(sf::Color(90, 240, 255));
  title.setOutlineThickness(1.0F);
  title.setOutlineColor(sf::Color(25, 105, 180));
  const auto titleBounds = title.getLocalBounds();
  title.setOrigin(titleBounds.left + titleBounds.width / 2.0F,
          titleBounds.top + titleBounds.height / 2.0F);
  title.setPosition(MenuCenterX, 165.0F);
  target.draw(title);

  sf::Text subtitle(subtitle_, font, 16U);
  subtitle.setFillColor(sf::Color(175, 190, 220));
  const auto subtitleBounds = subtitle.getLocalBounds();
  subtitle.setOrigin(subtitleBounds.left + subtitleBounds.width / 2.0F,
                       subtitleBounds.top + subtitleBounds.height / 2.0F);
  subtitle.setPosition(MenuCenterX, 218.0F);
  target.draw(subtitle);
}

void Menu::drawButtons(sf::RenderTarget& target, const sf::Font& font, bool hasFont) const {
  for (std::size_t idx = 0; idx < buttons_.size(); ++idx) {
    const bool selected = static_cast<int>(idx) == selectedIndex_;
    sf::RectangleShape button({buttons_[idx].bounds.width, buttons_[idx].bounds.height});
    button.setPosition(buttons_[idx].bounds.left, buttons_[idx].bounds.top);
    button.setFillColor(selected ? sf::Color(18, 65, 91, 235)
                                     : sf::Color(10, 20, 43, 220));
    button.setOutlineThickness(selected ? 2.4F : 1.2F);
    button.setOutlineColor(selected ? sf::Color(70, 235, 255)
                    : sf::Color(80, 110, 160));
    target.draw(button);

    if (!hasFont) {
      continue;
    }
    sf::Text label(buttons_[idx].label, font, 19U);
    label.setStyle(sf::Text::Bold);
    label.setFillColor(selected ? sf::Color::White : sf::Color(175, 200, 225));
    const auto bounds = label.getLocalBounds();
    label.setOrigin(bounds.left + bounds.width / 2.0F,
            bounds.top + bounds.height / 2.0F);
    label.setPosition(buttons_[idx].bounds.left + buttons_[idx].bounds.width / 2.0F,
             buttons_[idx].bounds.top + buttons_[idx].bounds.height / 2.0F - 2.0F);
    target.draw(label);
  }
}

void Menu::drawFooter(sf::RenderTarget& target, const sf::Font& font, bool hasFont) const {
  if (!hasFont) {
    return;
  }

  sf::Text footer("WASD / ARROWS  MOVE     SPACE  FIRE     ESC  PAUSE", font, 12U);
  footer.setFillColor(sf::Color(110, 135, 170));
  const auto bounds = footer.getLocalBounds();
  footer.setOrigin(bounds.left + bounds.width / 2.0F,
                     bounds.top + bounds.height / 2.0F);
  footer.setPosition(MenuCenterX, 690.0F);
  target.draw(footer);
}

}  // namespace space_survivor
