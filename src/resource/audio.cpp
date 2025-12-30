#include "audio.h"

#include <SDL3/SDL_properties.h>

#include <iostream>

namespace game::resource
{
  MIX_Mixer* Audio::mixer_get()
  {
    static auto mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    return mixer;
  }

  void Audio::set_gain(float gain)
  {
    auto mixer = mixer_get();
    MIX_SetMasterGain(mixer, gain);
  }

  void Audio::retain()
  {
    if (refCount) ++(*refCount);
  }

  void Audio::release()
  {
    if (refCount)
    {
      if (--(*refCount) == 0)
      {
        if (internal) MIX_DestroyAudio(internal);
        delete refCount;
      }
      refCount = nullptr;
    }
    internal = nullptr;
  }

  Audio::Audio(const std::filesystem::path& path)
  {
    internal = MIX_LoadAudio(mixer_get(), path.c_str(), true);
    if (internal)
    {
      refCount = new int(1);
      std::cout << "Initialized audio: '" << path.string() << "'\n";
    }
    else
    {
      std::cout << "Failed to initialize audio: '" << path.string() << "'\n";
    }
  }

  Audio::Audio(const Audio& other)
  {
    internal = other.internal;
    refCount = other.refCount;
    retain();
    track = nullptr;
  }

  Audio::Audio(Audio&& other) noexcept
  {
    internal = other.internal;
    track = other.track;
    refCount = other.refCount;

    other.internal = nullptr;
    other.track = nullptr;
    other.refCount = nullptr;
  }

  Audio& Audio::operator=(const Audio& other)
  {
    if (this != &other)
    {
      unload();
      internal = other.internal;
      refCount = other.refCount;
      retain();
    }
    return *this;
  }

  Audio& Audio::operator=(Audio&& other) noexcept
  {
    if (this != &other)
    {
      unload();
      internal = other.internal;
      track = other.track;
      refCount = other.refCount;

      other.internal = nullptr;
      other.track = nullptr;
      other.refCount = nullptr;
    }
    return *this;
  }

  void Audio::unload()
  {
    if (track)
    {
      MIX_DestroyTrack(track);
      track = nullptr;
    }
    release();
  }

  void Audio::play(bool isLoop)
  {
    if (!internal) return;

    auto mixer = mixer_get();

    if (track && MIX_GetTrackMixer(track) != mixer)
    {
      MIX_DestroyTrack(track);
      track = nullptr;
    }

    if (!track)
    {
      track = MIX_CreateTrack(mixer);
      if (!track) return;
    }

    MIX_SetTrackAudio(track, internal);

    SDL_PropertiesID options = 0;

    if (isLoop)
    {
      options = SDL_CreateProperties();
      if (options) SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    }

    MIX_PlayTrack(track, options);

    if (options) SDL_DestroyProperties(options);
  }

  void Audio::stop()
  {
    if (track) MIX_StopTrack(track, 0);
  }

  bool Audio::is_playing() const { return track && MIX_TrackPlaying(track); }

  Audio::~Audio() { unload(); }
  bool Audio::is_valid() const { return internal != nullptr; }
}
