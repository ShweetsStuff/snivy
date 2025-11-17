#pragma once

#include <unordered_set>

#include "../canvas.h"
#include "anm2.h"

namespace game::resource
{
  class Actor
  {

  public:
    anm2::Anm2 anm2{};
    glm::vec2 position{};
    float time{};
    bool isPlaying{};
    int animationIndex{-1};
    std::unordered_set<int> playedTriggers{};

    Actor(const std::filesystem::path&, glm::vec2);
    anm2::Animation* animation_get();
    anm2::Animation* animation_get(std::string&);
    int animation_index_get(anm2::Animation&);
    anm2::Item* item_get(anm2::Type, int = -1);
    anm2::Frame* trigger_get(int);
    anm2::Frame* frame_get(int, anm2::Type, int = -1);
    anm2::Frame frame_generate(anm2::Item&, float);
    void play(const std::string&);
    void tick();
    void render(Shader&, Canvas&);
  };
}