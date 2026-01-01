#include "actor.h"

#include "../util/map_.h"
#include "../util/math_.h"
#include "../util/unordered_map_.h"
#include "../util/vector_.h"

#include "../resource/audio.h"
#include "../resource/texture.h"

#include <glm/glm.hpp>
#include <iostream>

using namespace glm;
using namespace game::util;
using namespace game::anm2;

namespace game::resource
{
  Actor::Actor(Anm2* _anm2, vec2 _position, Mode mode, float time) : anm2(_anm2), position(_position)
  {
    if (anm2)
    {
      this->mode = mode;
      this->startTime = time;
      play(anm2->animations.defaultAnimation, mode, time);
    }
  }

  anm2::Animation* Actor::animation_get(int index)
  {
    if (!anm2) return nullptr;
    if (index == -1) index = animationIndex;
    if (anm2->animations.mapReverse.contains(index)) return &anm2->animations.items[index];
    return nullptr;
  }

  anm2::Animation* Actor::animation_get(const std::string& name)
  {
    if (!anm2) return nullptr;
    if (anm2->animations.map.contains(name)) return &anm2->animations.items[anm2->animations.map[name]];
    return nullptr;
  }

  bool Actor::is_playing(const std::string& name)
  {
    if (!anm2) return false;
    if (name.empty())
      return isPlaying;
    else
      return isPlaying && anm2->animations.map[name] == animationIndex;
  }

  int Actor::animation_index_get(const std::string& name)
  {
    if (!anm2) return -1;
    if (anm2->animations.map.contains(name)) return anm2->animations.map[name];
    return -1;
  }

  int Actor::item_id_get(const std::string& name, anm2::Type type)
  {
    if (!anm2 || (type != anm2::LAYER && type != anm2::NULL_)) return -1;

    if (type == anm2::LAYER)
    {
      for (int i = 0; i < anm2->content.layers.size(); i++)
        if (anm2->content.layers.at(i).name == name) return i;
    }
    else if (type == anm2::NULL_)
    {
      for (int i = 0; i < anm2->content.nulls.size(); i++)
        if (anm2->content.nulls.at(i).name == name) return i;
    }

    return -1;
  }

  anm2::Item* Actor::item_get(anm2::Type type, int id, int animationIndex)
  {
    if (!anm2) return nullptr;
    if (animationIndex == -1) animationIndex = this->animationIndex;
    if (auto animation = animation_get(animationIndex))
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

  int Actor::item_length(anm2::Item* item)
  {
    if (!item) return -1;

    int duration{};
    for (auto& frame : item->frames)
      duration += frame.duration;
    return duration;
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

  bool Actor::is_event(const std::string& event)
  {
    if (!anm2) return false;
    if (playedEventID == -1) return false;
    return event == anm2->content.events.at(playedEventID).name;
  }

  anm2::Frame Actor::frame_generate(anm2::Item& item, float time, anm2::Type type, int id)
  {
    anm2::Frame frame{};
    frame.isVisible = false;

    if (item.frames.empty()) return frame;

    time = time < 0.0f ? 0.0f : time;

    anm2::Frame* frameNext = nullptr;
    anm2::Frame frameNextCopy{};
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
        {
          frameNext = &item.frames[i + 1];
          frameNextCopy = *frameNext;
        }
        else
          frameNext = nullptr;
        break;
      }

      durationCurrent += frame.duration;
    }

    for (auto& override : overrides)
    {
      if (!override || !override->isEnabled) continue;

      if (id == override->destinationID)
      {
        switch (override->mode)
        {
          case Override::FRAME_ADD:
            if (override->frame.scale.has_value())
            {
              frame.scale += *override->frame.scale;
              if (frameNext) frameNextCopy.scale += *override->frame.scale;
            }
            if (override->frame.rotation.has_value())
            {
              frame.rotation += *override->frame.rotation;
              if (frameNext) frameNextCopy.rotation += *override->frame.rotation;
            }
            break;
          case Override::FRAME_SET:
            if (override->frame.scale.has_value())
            {
              frame.scale = *override->frame.scale;
              if (frameNext) frameNextCopy.scale = *override->frame.scale;
            }
            if (override->frame.rotation.has_value())
            {
              frame.rotation = *override->frame.rotation;
              if (frameNext) frameNextCopy.rotation = *override->frame.rotation;
            }
            break;
          case Override::ITEM_SET:
          default:
            if (override->animationIndex == -1) break;
            auto& animation = anm2->animations.items[override->animationIndex];
            auto overrideFrame = frame_generate(animation.layerAnimations[override->sourceID], override->time,
                                                anm2::LAYER, override->sourceID);
            frame.crop = overrideFrame.crop;
            break;
        }
      }
    }

    if (frame.isInterpolated && frameNext && frame.duration > 1)
    {
      auto interpolation = (time - durationCurrent) / (durationNext - durationCurrent);

      frame.rotation = glm::mix(frame.rotation, frameNextCopy.rotation, interpolation);
      frame.position = glm::mix(frame.position, frameNextCopy.position, interpolation);
      frame.scale = glm::mix(frame.scale, frameNextCopy.scale, interpolation);
      frame.colorOffset = glm::mix(frame.colorOffset, frameNextCopy.colorOffset, interpolation);
      frame.tint = glm::mix(frame.tint, frameNextCopy.tint, interpolation);
    }

    return frame;
  }

  void Actor::play(int index, Mode mode, float time, float speedMultiplier)
  {
    this->playedEventID = -1;
    this->playedTriggers.clear();

    if (!anm2) return;
    if (mode != FORCE_PLAY && this->animationIndex == index) return;
    if (!vector::in_bounds(anm2->animations.items, index)) return;
    this->speedMultiplier = speedMultiplier;
    this->previousAnimationIndex = animationIndex;
    this->animationIndex = index;
    this->time = time;
    if (mode == PLAY || mode == FORCE_PLAY) isPlaying = true;
  }

  void Actor::play(const std::string& name, Mode mode, float time, float speedMultiplier)
  {
    if (!anm2) return;
    if (anm2->animations.map.contains(name))
      play(anm2->animations.map.at(name), mode, time, speedMultiplier);
    else
      std::cout << "Animation \"" << name << "\" does not exist! Unable to play!\n";
  }

  void Actor::tick()
  {
    if (!anm2) return;
    if (!isPlaying) return;
    auto animation = animation_get();
    if (!animation) return;

    playedEventID = -1;

    for (auto& trigger : animation->triggers.frames)
    {
      if (!playedTriggers.contains(trigger.atFrame) && time >= trigger.atFrame)
      {
        if (auto sound = map::find(anm2->content.sounds, trigger.soundID)) sound->audio.play();
        playedTriggers.insert((int)trigger.atFrame);
        playedEventID = trigger.eventID;
      }
    }

    auto increment = (anm2->info.fps / 30.0f) * speedMultiplier;
    time += increment;

    if (time >= animation->frameNum)
    {
      if (animation->isLoop)
        time = 0.0f;
      else
        isPlaying = false;

      playedTriggers.clear();
    }

    for (auto& override : overrides)
    {
      if (!override->isEnabled || override->length < 0) continue;
      override->time += increment;
      if (override->time > override->length) override->isLoop ? override->time = 0.0f : override->isEnabled = false;
    }
  }

  glm::vec4 Actor::null_frame_rect(int nullID)
  {
    auto invalidRect = glm::vec4(0.0f / 0.0f);
    if (!anm2 || nullID == -1) return invalidRect;
    auto item = item_get(anm2::NULL_, nullID);
    if (!item) return invalidRect;

    auto animation = animation_get();
    if (!animation) return invalidRect;

    auto root = frame_generate(animation->rootAnimation, time, anm2::ROOT);

    for (auto& override : overrides)
    {
      if (!override || !override->isEnabled || override->type != anm2::ROOT) continue;

      switch (override->mode)
      {
        case Override::FRAME_ADD:
          if (override->frame.scale.has_value()) root.scale += *override->frame.scale;
          if (override->frame.rotation.has_value()) root.rotation += *override->frame.rotation;
          break;
        case Override::FRAME_SET:
          if (override->frame.scale.has_value()) root.scale = *override->frame.scale;
          if (override->frame.rotation.has_value()) root.rotation = *override->frame.rotation;
          break;
        default:
          break;
      }
    }

    auto frame = frame_generate(*item, time, anm2::NULL_, nullID);
    if (!frame.isVisible) return invalidRect;

    auto rootScale = math::to_unit(root.scale);
    auto frameScale = math::to_unit(frame.scale);
    auto combinedScale = rootScale * frameScale;
    auto scaledSize = NULL_SIZE * glm::abs(combinedScale);

    auto worldPosition = position + root.position + frame.position * rootScale;
    auto halfSize = scaledSize * 0.5f;

    return glm::vec4(worldPosition - halfSize, scaledSize);
  }

  void Actor::render(Shader& textureShader, Shader& rectShader, Canvas& canvas, Camera& camera, bool worldCoords)
  {
    if (!anm2) return;
    auto animation = animation_get();
    if (!animation) return;

    auto root = frame_generate(animation->rootAnimation, time, anm2::ROOT);

    for (auto& override : overrides)
    {
      if (!override || !override->isEnabled || override->type != anm2::ROOT) continue;

      switch (override->mode)
      {
        case Override::FRAME_ADD:
          if (override->frame.scale.has_value()) root.scale += *override->frame.scale;
          if (override->frame.rotation.has_value()) root.rotation += *override->frame.rotation;
          break;
        case Override::FRAME_SET:
          if (override->frame.scale.has_value()) root.scale = *override->frame.scale;
          if (override->frame.rotation.has_value()) root.rotation = *override->frame.rotation;
          break;
        default:
          break;
      }
    }

    auto rootModel = worldCoords ? math::quad_model_parent_get(root.position + position, root.pivot,
                                                               math::to_unit(root.scale), root.rotation)
                                 : camera.get_root_model(root.position + position, root.pivot,
                                                         math::to_unit(root.scale), root.rotation);

    for (auto& i : animation->layerOrder)
    {
      auto& layerAnimation = animation->layerAnimations[i];
      if (!layerAnimation.isVisible) continue;

      auto layer = map::find(anm2->content.layers, i);
      if (!layer) continue;

      auto spritesheet = map::find(anm2->content.spritesheets, layer->spritesheetID);
      if (!spritesheet) continue;

      auto frame = frame_generate(layerAnimation, time, anm2::LAYER, i);
      if (!frame.isVisible) continue;

      auto model =
          math::quad_model_get(frame.size, frame.position, frame.pivot, math::to_unit(frame.scale), frame.rotation);
      model = rootModel * model;

      auto& texture = spritesheet->texture;
      if (!texture.is_valid()) return;

      auto tint = frame.tint * root.tint;
      auto colorOffset = frame.colorOffset + root.colorOffset;

      auto uvMin = frame.crop / vec2(texture.size);
      auto uvMax = (frame.crop + frame.size) / vec2(texture.size);
      auto uvVertices = math::uv_vertices_get(uvMin, uvMax);

      canvas.texture_render(textureShader, texture.id, model, tint, colorOffset, uvVertices.data());
    }

    if (isShowNulls)
    {
      for (int i = 0; i < animation->nullAnimations.size(); i++)
      {
        auto& nullAnimation = animation->nullAnimations[i];
        if (!nullAnimation.isVisible) continue;

        auto frame = frame_generate(nullAnimation, time, anm2::NULL_, i);
        if (!frame.isVisible) continue;

        auto model = math::quad_model_get(frame.scale, frame.position, frame.scale * 0.5f, vec2(1.0f), frame.rotation);
        model = rootModel * model;

        canvas.rect_render(rectShader, model);
      }
    }
  }

  void Actor::consume_played_event() { playedEventID = -1; }
};
