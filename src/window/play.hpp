#pragma once

#include "../character.h"
#include "../game_data.h"
#include "../resources.h"

#include "inventory.h"
#include "text.h"

#include <imgui.h>

namespace game::window
{
  class Play
  {

    static constexpr ImVec4 LINE_COLOR = ImVec4(1, 1, 1, 1);
    static constexpr ImVec4 RECT_COLOR = ImVec4(0, 1, 0, 1);
    static constexpr ImVec4 BG_COLOR = ImVec4(0, 1, 0, 0.1);
    static constexpr ImVec4 PERFECT_COLOR = ImVec4(1, 1, 1, 0.75);
    static constexpr auto LINE_HEIGHT = 2.0f;
    static constexpr auto LINE_WIDTH_BONUS = 10.0f;

    static constexpr auto RANGE_BASE = 0.75f;
    static constexpr auto RANGE_MIN = 0.10f;
    static constexpr auto RANGE_SCORE_BONUS = 0.0005f;

    static constexpr auto SPEED_BASE = 0.005f;
    static constexpr auto SPEED_MAX = 0.075f;
    static constexpr auto SPEED_SCORE_BONUS = 0.000025f;

    static constexpr auto EXPONENTIAL_LEVEL_MIN = 3;
    static constexpr auto EXPONENTIAL_CHANCE_BASE = 0.25f;
    static constexpr auto RANGE_AREA_COUNT = 5;
    static constexpr auto BONUS_RANGE_CHANCE_BASE = 25.0f;

    static constexpr auto END_TIMER_MAX = 30;
    static constexpr auto END_TIMER_MISS_MAX = 90;

    static constexpr auto HIGH_SCORE_BIG = 999;

    static constexpr auto TOAST_MESSAGE_SPEED = 1.0f;

    static constexpr auto REWARD_SCORE_BONUS = 0.01f;
    static constexpr auto REWARD_GRADE_BONUS = 0.05f;
    static constexpr auto REWARD_LEVEL_BONUS = 0.25f;

    static constexpr auto REWARD_GRADE_CHANCE_BONUS = 1.0f;
    static constexpr auto REWARD_LEVEL_CHANCE_BONUS = 1.0f;

    static constexpr auto ITEM_SIZE = glm::vec2(48.0f, 48.0f);

  public:
#define GRADES                                                                                                         \
  X(MISS, "Miss!", "Misses", 0, -0.05f)                                                                                \
  X(OK, "OK", "OKs", 1, 0.40f)                                                                                         \
  X(GOOD, "Good", "Goods", 2, 0.65f)                                                                                   \
  X(GREAT, "Great", "Greats", 3, 0.85f)                                                                                \
  X(EXCELLENT, "Excellent!", "Excellents", 4, 0.95f)                                                                   \
  X(PERFECT, "PERFECT!", "Perfects", 5, 1.00f)

    enum Grade
    {
#define X(symbol, name, nameStats, value, weight) symbol,
      GRADES
#undef X
          GRADE_COUNT
    };

    static constexpr const char* GRADE_STRINGS[] = {
#define X(symbol, name, nameStats, value, weight) name,
        GRADES
#undef X
    };

    static constexpr const char* GRADE_STATS_STRINGS[] = {
#define X(symbol, name, nameStats, value, weight) nameStats,
        GRADES
#undef X
    };

    static constexpr float GRADE_WEIGHTS[] = {
#define X(symbol, name, nameStats, value, weight) weight,
        GRADES
#undef X
    };

    static constexpr int GRADE_VALUES[] = {
#define X(symbol, name, nameStats, value, weight) value,
        GRADES
#undef X
    };

#undef GRADES

    struct Range
    {
      float min{};
      float max{};
    };

    struct Challenge
    {
      Range range{};
      float speed{};
      float tryValue{};
      int level{};
    };

    struct ToastMessage
    {
      std::string message{};
      ImVec2 position;
      int time{};
      int timeMax{};
    };

    struct ToastItem
    {
      Item::Type type{};
      ImVec2 position;
      float velocityY;
    };

    Challenge challenge{};
    Challenge queuedChallenge{};
    float tryValue{};

    int score{};
    int combo{};
    int comboBest{};
    int highScore{};
    int highScoreStart{};
    int endTimer{};
    int endTimerMax{};
    bool isActive{true};
    bool isHighScoreBigAchieved{false};
    bool isHighScoreAchieved{false};
    bool isGameOver{};

    int totalPlays{};

    std::unordered_map<Grade, int> gradeCounts{};

    std::vector<ToastMessage> toastMessages{};
    std::vector<ToastItem> toastItems{};

    Play();
    Challenge challenge_generate(int);
    void update(Resources&, Character&, Inventory&, GameData&, Text&);
    float accuracy_score_get();
  };
}
