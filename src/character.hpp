#pragma once

#include "resource/actor.h"
#include "types.h"

namespace game
{
  class Character : public resource::Actor
  {
  public:
    static constexpr auto LAYER_TAIL = "Tail";
    static constexpr auto LAYER_TORSO = "Torso";
    static constexpr auto LAYER_HEAD = "Head";
    static constexpr auto LAYER_MOUTH = "Mouth";
    static constexpr auto LAYER_TALK = "Talk";
    static constexpr auto LAYER_BLINK = "Blink";

    static constexpr auto NULL_MOUTH = "Mouth";
    static constexpr auto NULL_HEAD = "Head";
    static constexpr auto NULL_BELLY = "Belly";
    static constexpr auto NULL_TAIL = "Tail";

    static constexpr auto EVENT_EAT = "Eat";

    static constexpr auto BLINK_CHANCE = 0.5f;

    static constexpr auto PAT_CHANCE = 25.0f;
    static constexpr auto BURP_SMALL_CHANCE = 20.0f;
    static constexpr auto BURP_BIG_CHANCE = 10.0f;

    static constexpr auto GURGLE_CHANCE = 0.1f;
    static constexpr auto GURGLE_CHANCE_BONUS = 0.3f;

    static constexpr auto CAPACITY_BASE = 500.0f;

    static constexpr auto CALORIE_TO_KG = 1.0 / 1000.0f;
    static constexpr auto CAPACITY_OVERSTUFFED_LIMIT_MULTIPLIER = 1.5f;

    static constexpr auto SCALE_BONUS_MAX = 10.0f;

    static constexpr auto PAT_LENGTH = 5;
    static constexpr auto PAT_SCALE_RANGE = 5;

    static constexpr auto EAT_SPEED_MULTIPLIER_MIN = 1.0f;
    static constexpr auto EAT_SPEED_MULTIPLIER_MAX = 3.0f;

    static constexpr auto DIGESTION_RATE_MIN = 0.00f;
    static constexpr auto DIGESTION_RATE_MAX = 0.25f;
    static constexpr auto DIGESTION_RATE_BASE = 0.05f;
    static constexpr auto DIGESTION_MAX = 100.0f;
    static constexpr auto DIGESTION_TIMER_MAX = 60;
    static constexpr auto DIGESTION_RUB_BONUS = 0.01f;

    static constexpr auto CAPACITY_OVER_BONUS = 0.1f;

    static constexpr auto WEIGHT_STAGE_MAX = 5;

    static constexpr float WEIGHT_THRESHOLDS[] = {
        8.1f, 15.0f, 30.0f, 50.0f, 75.0f,
    };

    static constexpr auto MOUTH_SIZE = glm::vec2(50.0f, 50.0f);

    enum State
    {
      APPEAR,
      IDLE,
      EAGER,
      SHOCKED,
      EAT,
      CRY,
      ANGRY,
      BURP_SMALL,
      BURP_BIG,
      PAT,
      HEAD_RUB,
      BELLY_RUB,
      TAIL_RUB,
      STAGE_UP
    };

#define ANIMATIONS_LIST                                                                                                \
  X(ANIMATION_NEUTRAL, "Neutral")                                                                                      \
  X(ANIMATION_NEUTRAL_FULL, "NeutralFull")                                                                             \
  X(ANIMATION_SHOCKED, "Shocked")                                                                                      \
  X(ANIMATION_EAT, "Eat")                                                                                              \
  X(ANIMATION_ANGRY, "Angry")                                                                                          \
  X(ANIMATION_EAGER, "Eager")                                                                                          \
  X(ANIMATION_CRY, "Cry")                                                                                              \
  X(ANIMATION_PAT, "Pat")                                                                                              \
  X(ANIMATION_BURP_SMALL, "BurpSmall")                                                                                 \
  X(ANIMATION_BURP_BIG, "BurpBig")                                                                                     \
  X(ANIMATION_HEAD_RUB, "HeadRub")                                                                                     \
  X(ANIMATION_HEAD_RUB_FULL, "HeadRubFull")                                                                            \
  X(ANIMATION_BELLY_RUB, "BellyRub")                                                                                   \
  X(ANIMATION_BELLY_RUB_FULL, "BellyRubFull")                                                                          \
  X(ANIMATION_TAIL_RUB, "TailRub")                                                                                     \
  X(ANIMATION_TAIL_RUB_FULL, "TailRubFull")                                                                            \
  X(ANIMATION_STAGE_UP, "StageUp")

    enum AnimationType
    {
#define X(symbol, string) symbol,
      ANIMATIONS_LIST
#undef X
    };

    static constexpr const char* ANIMATIONS[] = {
#define X(symbol, string) string,
        ANIMATIONS_LIST
#undef X
    };

    float weight{WEIGHT_THRESHOLDS[0]};
    int weightStage{0};
    int previousWeightStage{0};
    float highestWeight{};
    float calories{};
    float capacity{CAPACITY_BASE};
    float digestionProgress{};
    float digestionRate{DIGESTION_RATE_BASE};

    float totalWeightGained{};
    float totalCaloriesConsumed{};
    int foodItemsEaten{};
    int digestionCount{};

    bool isJustDigestionStart{};
    bool isJustDigestionEnd{};
    bool isJustStageUp{};
    bool isForceStageUp{};
    bool isJustFinalThreshold{};
    bool isFinalThresholdReached{};
    bool isDigesting{};
    bool isJustAppeared{};
    int digestionTimer{};

    int blinkLayerID{-1};
    int headLayerID{-1};
    int tailLayerID{-1};
    int talkLayerID{-1};
    int mouthLayerID{-1};
    int torsoLayerID{-1};

    int mouthNullID{-1};
    int headNullID{-1};
    int bellyNullID{-1};
    int tailNullID{-1};

    bool isFinishedFood{};

    float eatSpeedMultiplier{EAT_SPEED_MULTIPLIER_MIN};

    State state{APPEAR};

    Override blinkOverride{};
    Override talkOverride{};
    Override torsoCapacityScale{};
    Override tailCapacityScale{};

    Character(anm2::Anm2*, glm::ivec2);
    void talk();
    void blink();
    void tick();
    void digestion_start();
    void digestion_end();
    void state_set(State, bool = false);
    glm::vec4 mouth_rect_get();
    glm::vec4 belly_rect_get();
    glm::vec4 head_rect_get();
    glm::vec4 tail_rect_get();
    float weight_get(MeasurementSystem = METRIC);
    float weight_threshold_get(int, MeasurementSystem = METRIC);
    float weight_threshold_current_get(MeasurementSystem = METRIC);
    float weight_threshold_next_get(MeasurementSystem = METRIC);
    float progress_to_next_weight_threshold_get();
    float over_capacity_percent_get();
    float over_capacity_calories_get();
    float digestion_rate_second_get();
    bool is_max_capacity();
    bool is_over_capacity();
    float max_capacity();
    std::string animation_name_convert(const std::string&);
  };
}
