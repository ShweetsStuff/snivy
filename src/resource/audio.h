#pragma once

#include <SDL3_mixer/SDL_mixer.h>
#include <filesystem>

namespace game::resource
{
  class Audio
  {
    MIX_Audio* internal{nullptr};
    MIX_Track* track{nullptr};
    int* refCount{nullptr};
    static MIX_Mixer* mixer_get();
    void unload();
    void retain();
    void release();

  public:
    Audio() = default;
    Audio(const std::filesystem::path&);
    Audio(const Audio&);
    Audio(Audio&&) noexcept;
    Audio& operator=(const Audio&);
    Audio& operator=(Audio&&) noexcept;
    ~Audio();
    bool is_valid() const;
    void play(bool isLoop = false);
    void stop();
    bool is_playing() const;
    static void set_gain(float vol);
  };
}
