#include "character.h"

#include <format>

#include "types.h"
#include "util/math_.h"

using namespace game::util;
using namespace game::anm2;
using namespace glm;

namespace game
{
  float Character::max_capacity() { return capacity * CAPACITY_OVERSTUFFED_LIMIT_MULTIPLIER; }

  float Character::over_capacity_calories_get()
  {
    if (calories < capacity) return 0.0f;
    return (calories - capacity);
  }
  float Character::over_capacity_percent_get()
  {
    if (calories < capacity) return 0.0f;
    return (calories - capacity) / (max_capacity() - capacity);
  }

  float Character::digestion_rate_second_get() { return digestionRate * 60; }

  bool Character::is_over_capacity() { return calories + 1 >= capacity; }
  bool Character::is_max_capacity() { return calories >= max_capacity(); }

  void Character::talk()
  {
    if (auto talkItem = item_get(anm2::LAYER, talkLayerID))
    {
      talkOverride = {.animationIndex = animationIndex,
                      .sourceID = talkLayerID,
                      .destinationID = mouthLayerID,
                      .length = (float)item_length(talkItem),
                      .isLoop = true};
    }
    else
      talkOverride.isEnabled = false;
  }

  void Character::blink()
  {
    if (auto blinkItem = item_get(anm2::LAYER, blinkLayerID))
    {
      blinkOverride = {.animationIndex = animationIndex,
                       .sourceID = blinkLayerID,
                       .destinationID = headLayerID,
                       .length = (float)item_length(blinkItem)};
    }
    else
      blinkOverride.isEnabled = false;
  }

  float Character::weight_get(MeasurementSystem system) { return system == IMPERIAL ? weight * KG_TO_LB : weight; }

  float Character::weight_threshold_get(int stage, MeasurementSystem system)
  {
    stage = glm::clamp(stage, 0, WEIGHT_STAGE_MAX);
    return system == IMPERIAL ? WEIGHT_THRESHOLDS[stage] * KG_TO_LB : WEIGHT_THRESHOLDS[stage];
  }

  float Character::weight_threshold_current_get(MeasurementSystem system)
  {
    return weight_threshold_get(weightStage, system);
  }

  float Character::weight_threshold_next_get(MeasurementSystem system)
  {
    auto nextStage = glm::clamp(0, weightStage + 1, WEIGHT_STAGE_MAX);
    return weight_threshold_get(nextStage, system);
  }

  float Character::progress_to_next_weight_threshold_get()
  {
    if (weightStage >= WEIGHT_STAGE_MAX - 1) return 0.0f;
    return (weight - weight_threshold_current_get()) / (weight_threshold_next_get() - weight_threshold_current_get());
  }

  vec4 Character::mouth_rect_get() { return null_frame_rect(mouthNullID); }
  vec4 Character::head_rect_get() { return null_frame_rect(headNullID); }
  vec4 Character::belly_rect_get() { return null_frame_rect(bellyNullID); }
  vec4 Character::tail_rect_get() { return null_frame_rect(tailNullID); }

  Character::Character(Anm2* _anm2, glm::ivec2 _position) : Actor(_anm2, _position)
  {
    talkLayerID = item_id_get(LAYER_TALK);
    blinkLayerID = item_id_get(LAYER_BLINK);
    headLayerID = item_id_get(LAYER_HEAD);
    mouthLayerID = item_id_get(LAYER_MOUTH);
    torsoLayerID = item_id_get(LAYER_TORSO);
    tailLayerID = item_id_get(LAYER_TAIL);

    mouthNullID = item_id_get(NULL_MOUTH, anm2::NULL_);
    headNullID = item_id_get(NULL_HEAD, anm2::NULL_);
    bellyNullID = item_id_get(NULL_BELLY, anm2::NULL_);
    tailNullID = item_id_get(NULL_TAIL, anm2::NULL_);

    torsoCapacityScale = {.destinationID = torsoLayerID, .mode = Override::FRAME_ADD};
    tailCapacityScale = {.destinationID = tailLayerID, .mode = Override::FRAME_ADD};
    torsoCapacityScale.frame.scale = glm::vec2();
    tailCapacityScale.frame.scale = glm::vec2();

    overrides.emplace_back(&talkOverride);
    overrides.emplace_back(&blinkOverride);
    overrides.emplace_back(&torsoCapacityScale);
    overrides.emplace_back(&tailCapacityScale);
  }

  void Character::digestion_start()
  {
    isDigesting = true;
    digestionTimer = DIGESTION_TIMER_MAX;
    isJustDigestionStart = true;
  }

  void Character::digestion_end()
  {
    auto increment = calories * CALORIE_TO_KG;
    weight += increment;
    totalWeightGained += increment;
    isForceStageUp = false;

    if (is_over_capacity()) capacity += over_capacity_percent_get() * capacity * CAPACITY_OVER_BONUS;

    calories = 0;
    digestionProgress = 0;
    digestionTimer = 0;
    digestionCount++;
    isDigesting = false;
    isJustDigestionEnd = true;
  }

  void Character::tick()
  {
    Actor::tick();

    isJustDigestionStart = false;
    isJustDigestionEnd = false;
    isJustStageUp = false;
    isJustFinalThreshold = false;

    auto animation = animation_get();
    if (animation && !animation->isLoop && !isPlaying)
    {
      if (state == APPEAR) isJustAppeared = true;
      if (state == STAGE_UP && weightStage == WEIGHT_STAGE_MAX - 1 && !isForceStageUp) isJustFinalThreshold = true;
      state_set(IDLE, true);
    }

    if (isDigesting)
    {
      digestionTimer--;
      if (digestionTimer <= 0) digestion_end();
    }
    else if (calories > 0)
    {
      digestionProgress += digestionRate;
      if (digestionProgress >= DIGESTION_MAX) digestion_start();
    }

    if (math::random_percent_roll(BLINK_CHANCE)) blink();

    auto progress = calories / max_capacity();
    auto weightPercent = progress_to_next_weight_threshold_get();
    auto capacityPercent = isDigesting ? ((float)digestionTimer / DIGESTION_TIMER_MAX) * progress : progress;
    auto scaleBonus =
        vec2(glm::min(SCALE_BONUS_MAX * ((capacityPercent * 0.5f) + (weightPercent * 0.5f)), SCALE_BONUS_MAX));

    torsoCapacityScale.frame.scale = glm::max(vec2(), scaleBonus);
    tailCapacityScale.frame.scale = glm::max(vec2(), scaleBonus);

    if (!isForceStageUp)
    {
      for (int i = 0; i < WEIGHT_STAGE_MAX; i++)
      {
        if (weight >= WEIGHT_THRESHOLDS[i])
        {
          if (i == previousWeightStage + 1)
          {
            state_set(STAGE_UP);
            isJustStageUp = true;
            weightStage = i;
            break;
          }
        }
        else if (weight < WEIGHT_THRESHOLDS[i])
          break;
      }
    }

    if (weight > highestWeight) highestWeight = weight;

    previousWeightStage = weightStage;
  }

  std::string Character::animation_name_convert(const std::string& name)
  {
    return std::format("{}{}", name, weightStage);
  }

  void Character::state_set(State state, bool isForce)
  {
    if (this->state == state && !isForce) return;
    this->state = state;
    AnimationType type{ANIMATION_NEUTRAL};
    auto speedMultiplier = 1.0f;

    switch (this->state)
    {
      case IDLE:
        if (is_over_capacity())
          type = ANIMATION_NEUTRAL_FULL;
        else
          type = ANIMATION_NEUTRAL;
        break;
      case EAGER:
        type = ANIMATION_EAGER;
        break;
      case CRY:
        type = ANIMATION_CRY;
        break;
      case SHOCKED:
        type = ANIMATION_SHOCKED;
        break;
      case EAT:
        type = ANIMATION_EAT;
        speedMultiplier = eatSpeedMultiplier;
        break;
      case ANGRY:
        type = ANIMATION_ANGRY;
        break;
      case PAT:
        type = ANIMATION_PAT;
        break;
      case BURP_SMALL:
        type = ANIMATION_BURP_SMALL;
        break;
      case BURP_BIG:
        type = ANIMATION_BURP_BIG;
        break;
      case HEAD_RUB:
        if (is_over_capacity())
          type = ANIMATION_HEAD_RUB_FULL;
        else
          type = ANIMATION_HEAD_RUB;
        break;
      case BELLY_RUB:
        if (is_over_capacity())
          type = ANIMATION_BELLY_RUB_FULL;
        else
          type = ANIMATION_BELLY_RUB;
        break;
      case TAIL_RUB:
        if (is_over_capacity())
          type = ANIMATION_TAIL_RUB_FULL;
        else
          type = ANIMATION_TAIL_RUB;
        break;
      case STAGE_UP:
        type = ANIMATION_STAGE_UP;
        break;
      default:
        break;
    };

    play(animation_name_convert(ANIMATIONS[type]), PLAY, 0.0f, speedMultiplier);
  }
}