#include "AudioManager.hpp"

#include <algorithm>
#include <array>
#include <cstddef>

namespace space_survivor {
namespace {
constexpr std::array<const char*, 5U> SoundFiles{
  "shoot.wav", "explosion.wav", "damage.wav", "powerup.wav", "menu.wav"
};

[[nodiscard]] std::size_t soundIndex(SoundType type) noexcept {
  return static_cast<std::size_t>(type);
}
}  // namespace

void AudioManager::load(const std::string& assetDirectory) {
  // AUDIT-013: Separate sound instances allow overlapping effects.
  for (std::size_t idx = 0; idx < SoundFiles.size(); ++idx) {
    const std::string path = assetDirectory + "/" + SoundFiles[idx];
    loaded_[idx] = buffers_[idx].loadFromFile(path);
    if (loaded_[idx]) {
      sounds_[idx].setBuffer(buffers_[idx]);
      sounds_[idx].setVolume(muted_ ? 0.0F : volume_);
    }
  }

  musicLoaded_ = music_.openFromFile(assetDirectory + "/background.ogg");
  if (musicLoaded_) {
    music_.setLoop(true);
    music_.setVolume(muted_ ? 0.0F : volume_);
    if (!muted_) {
      music_.play();
    }
  }
}

void AudioManager::play(SoundType type) {
  const std::size_t idx = soundIndex(type);
  if (idx >= sounds_.size() || !loaded_[idx] || muted_) {
    return;
  }
  sounds_[idx].play();
}

void AudioManager::setVolume(float volume) noexcept {
  volume_ = std::clamp(volume, 0.0F, 100.0F);
  const float effectiveVolume = muted_ ? 0.0F : volume_;
  for (std::size_t idx = 0; idx < sounds_.size(); ++idx) {
    if (loaded_[idx]) {
      sounds_[idx].setVolume(effectiveVolume);
    }
  }
  if (musicLoaded_) {
    music_.setVolume(effectiveVolume);
  }
}

void AudioManager::toggleMute() noexcept {
  muted_ = !muted_;
  setVolume(volume_);
  if (!musicLoaded_) {
    return;
  }
  if (muted_) {
    music_.pause();
  } else if (music_.getStatus() != sf::Music::Playing) {
    music_.play();
  }
}

}  // namespace space_survivor
