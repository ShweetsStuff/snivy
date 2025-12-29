#pragma once

#include <unordered_set>

#include "../canvas.h"
#include "anm2.h"

namespace game::resource
{
  class Actor
  {

  public:
    static constexpr auto NULL_SIZE = glm::vec2(100, 100);

    enum Mode
    {
      PLAY,
      SET,
      FORCE_PLAY
    };

    struct Override
    {
      enum Mode
      {
        ITEM_SET,
        FRAME_ADD,
        FRAME_SET
      };

      anm2::FrameOptional frame{};
      int animationIndex{-1};
      int sourceID{-1};
      int destinationID{-1};
      float length{-1.0f};
      bool isLoop{false};
      Mode mode{ITEM_SET};
      anm2::Type type{anm2::LAYER};

      bool isEnabled{true};
      float time{};
    };

    anm2::Anm2* anm2{};
    glm::vec2 position{};
    float time{};
    bool isPlaying{};
    bool isShowNulls{};
    int animationIndex{-1};
    int previousAnimationIndex{-1};
    int lastPlayedAnimationIndex{-1};
    int playedEventID{-1};
    Mode mode{PLAY};
    float startTime{};
    float speedMultiplier{};

    std::unordered_set<int> playedTriggers{};
    std::vector<Override*> overrides{};

    Actor(anm2::Anm2*, glm::vec2, Mode = PLAY, float = 0.0f);
    anm2::Animation* animation_get(int = -1);
    anm2::Animation* animation_get(const std::string&);
    int animation_index_get(const std::string&);
    anm2::Item* item_get(anm2::Type, int = -1, int = -1);
    int item_length(anm2::Item*);
    anm2::Frame* trigger_get(int);
    anm2::Frame* frame_get(int, anm2::Type, int = -1);
    int item_id_get(const std::string&, anm2::Type = anm2::LAYER);
    anm2::Frame frame_generate(anm2::Item&, float, anm2::Type, int = -1);
    void play(const std::string&, Mode = PLAY, float = 0.0f, float = 1.0f);
    void play(int, Mode = PLAY, float = 0.0f, float = 1.0f);
    bool is_event(const std::string& event);
    void tick();
    bool is_playing(const std::string& name = {});
    void render(Shader& textureShader, Shader& rectShader, Canvas&);
    glm::vec4 null_frame_rect(int = -1);
    void consume_played_event();
  };
}