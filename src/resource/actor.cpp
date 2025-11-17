#include "actor.h"

#include "../util/map_.h"
#include "../util/math_.h"
#include "../util/unordered_map_.h"
#include "../util/vector_.h"

#include "../resource/audio.h"
#include "../resource/texture.h"

#include <glm/glm.hpp>

using namespace glm;
using namespace game::util;
using namespace game::anm2;

namespace game::resource
{
  std::shared_ptr<void> texture_callback(const std::filesystem::path& path) { return std::make_shared<Texture>(path); }
  std::shared_ptr<void> sound_callback(const std::filesystem::path& path) { return std::make_shared<Audio>(path); }

  Actor::Actor(const std::filesystem::path& path, vec2 position) : anm2(path, texture_callback, sound_callback)
  {
    this->position = position;
    play(anm2.animations.defaultAnimation);
  }

  anm2::Animation* Actor::animation_get() { return vector::find(anm2.animations.items, animationIndex); }

  anm2::Item* Actor::item_get(anm2::Type type, int id)
  {
    if (auto animation = animation_get())
    {
      switch (type)
      {
        case anm2::ROOT:
          return &animation->rootAnimation;
          break;
        case anm2::LAYER:
          return unordered_map::find(animation->layerAnimations, id);
        case anm2::NULL_:
          return map::find(animation->nullAnimations, id);
          break;
        case anm2::TRIGGER:
          return &animation->triggers;
        default:
          return nullptr;
      }
    }

    return nullptr;
  }

  anm2::Frame* Actor::trigger_get(int atFrame)
  {
    if (auto item = item_get(anm2::TRIGGER))
      for (auto& trigger : item->frames)
        if (trigger.atFrame == atFrame) return &trigger;

    return nullptr;
  }

  anm2::Frame* Actor::frame_get(int index, anm2::Type type, int id)
  {
    if (auto item = item_get(type, id)) return vector::find(item->frames, index);
    return nullptr;
  }

  anm2::Frame Actor::frame_generate(anm2::Item& item, float time)
  {
    anm2::Frame frame{};
    frame.isVisible = false;

    if (item.frames.empty()) return frame;

    time = time < 0.0f ? 0.0f : time;

    anm2::Frame* frameNext = nullptr;
    int durationCurrent = 0;
    int durationNext = 0;

    for (int i = 0; i < item.frames.size(); i++)
    {
      anm2::Frame& checkFrame = item.frames[i];

      frame = checkFrame;

      durationNext += frame.duration;

      if (time >= durationCurrent && time < durationNext)
      {
        if (i + 1 < (int)item.frames.size())
          frameNext = &item.frames[i + 1];
        else
          frameNext = nullptr;
        break;
      }

      durationCurrent += frame.duration;
    }

    if (frame.isInterpolated && frameNext && frame.duration > 1)
    {
      auto interpolation = (time - durationCurrent) / (durationNext - durationCurrent);

      frame.rotation = glm::mix(frame.rotation, frameNext->rotation, interpolation);
      frame.position = glm::mix(frame.position, frameNext->position, interpolation);
      frame.scale = glm::mix(frame.scale, frameNext->scale, interpolation);
      frame.colorOffset = glm::mix(frame.colorOffset, frameNext->colorOffset, interpolation);
      frame.tint = glm::mix(frame.tint, frameNext->tint, interpolation);
    }

    return frame;
  }

  void Actor::play(const std::string& name)
  {
    for (int i = 0; i < anm2.animations.items.size(); i++)
    {
      if (anm2.animations.items[i].name == name)
      {
        animationIndex = i;
        time = 0.0f;
        isPlaying = true;
        break;
      }
    }
  }

  void Actor::tick()
  {
    if (!isPlaying) return;
    auto animation = animation_get();
    if (!animation) return;

    time += anm2.info.fps / 30.0f;

    auto intTime = (int)time;

    if (auto trigger = trigger_get(intTime))
    {
      if (!playedTriggers.contains(intTime))
      {
        if (auto sound = map::find(anm2.content.sounds, trigger->soundID)) sound->audio.play();
        playedTriggers.insert(intTime);
      }
    }

    if (time >= animation->frameNum)
    {
      if (animation->isLoop)
        time = 0.0f;
      else
        isPlaying = false;

      playedTriggers.clear();
    }
  }

  void Actor::render(Shader& shader, Canvas& canvas)
  {
    auto animation = animation_get();
    if (!animation) return;

    auto root = frame_generate(animation->rootAnimation, time);
    auto rootModel = math::quad_model_parent_get(root.position + position, root.pivot,
                                                 math::percent_to_unit(root.scale), root.rotation);

    for (auto& i : animation->layerOrder)
    {
      auto& layerAnimation = animation->layerAnimations[i];
      if (!layerAnimation.isVisible) continue;

      auto layer = map::find(anm2.content.layers, i);
      if (!layer) continue;

      auto spritesheet = map::find(anm2.content.spritesheets, layer->spritesheetID);
      if (!spritesheet) continue;

      auto frame = frame_generate(layerAnimation, time);
      if (!frame.isVisible) continue;

      auto model = math::quad_model_get(frame.size, frame.position, frame.pivot, math::percent_to_unit(frame.scale),
                                        frame.rotation);
      model = rootModel * model;

      auto& texture = spritesheet->texture;
      if (!texture.is_valid()) return;

      auto uvMin = frame.crop / vec2(texture.size);
      auto uvMax = (frame.crop + frame.size) / vec2(texture.size);
      auto uvVertices = math::uv_vertices_get(uvMin, uvMax);

      canvas.texture_render(shader, texture.id, model, frame.tint, frame.colorOffset, uvVertices.data());
    }
  }
};
