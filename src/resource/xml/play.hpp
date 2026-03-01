#pragma once

#include <string>

#include "util.hpp"

#include "dialogue.hpp"

namespace game::resource::xml
{
  class Play
  {
  public:
    struct Grade
    {
      std::string name{};
      std::string namePlural{};
      int value{};
      float weight{};
      bool isFailure{};
      Audio sound{};
      Dialogue::PoolReference pool{};
    };

    struct Sounds
    {
      SoundEntryCollection fall{};
      SoundEntryCollection highScore{};
      SoundEntryCollection highScoreLoss{};
      SoundEntryCollection rewardScore{};
      SoundEntryCollection scoreLoss{};
    };

    Sounds sounds{};
    std::vector<Grade> grades{};

    float rewardScoreBonus{0.01};
    float rewardGradeBonus{0.05};
    float speedMin{0.005};
    float speedMax{0.075};
    float speedScoreBonus{0.000025f};
    float rangeBase{0.75};
    float rangeMin{0.10};
    float rangeScoreBonus{0.0005};
    int endTimerMax{20};
    int endTimerFailureMax{60};
    int rewardScore{999};

    bool isValid{};

    Play() = default;
    Play(const util::physfs::Path&, Dialogue&);

    bool is_valid() const;
  };
}
