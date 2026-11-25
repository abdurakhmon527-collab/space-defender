#pragma once

#include "Types.hpp"

#include <SFML/Audio.hpp>

#include <array>
#include <string>

namespace space_survivor {

class AudioManager {
public:
  AudioManager() = default;

  void load(const std::string& assetDirectory = "assets/audio");
  void play(SoundType type);
  void setVolume(float volume) noexcept;
  [[nodiscard]] float getVolume() const noexcept { return volume_; }
  [[nodiscard]] bool isMuted() const noexcept { return muted_; }
  void toggleMute() noexcept;

private:
  static constexpr std::size_t SoundCount = 5U;
  std::array<sf::SoundBuffer, SoundCount> buffers_;
  std::array<sf::Sound, SoundCount> sounds_;
  std::array<bool, SoundCount> loaded_{};
  sf::Music music_;
  bool musicLoaded_{false};
  float volume_{45.0F};
  bool muted_{false};
};

}  // namespace space_survivor
