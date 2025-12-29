#include "stats.h"
#include <format>

using namespace game::resource;

namespace game::window
{
  void Stats::update(Resources& resources, GameData& gameData, Play& play, Character& character)
  {
    ImGui::PushFont(resources.font.get(), Font::BIG);
    ImGui::Text("Snivy");
    ImGui::PopFont();

    ImGui::Separator();

    auto& system = gameData.measurementSystem;
    auto weight = character.weight_get(system);
    auto weightUnit = system == MeasurementSystem::IMPERIAL ? "lbs" : "kg";

    ImGui::Text("Weight: %0.2f %s (Stage: %i)", weight, weightUnit, character.weightStage + 1);
    ImGui::Text("Capacity: %0.0f (Max: %0.0f)", character.capacity, character.max_capacity());
    ImGui::Text("Digestion Rate: %0.2f%%/sec", character.digestion_rate_second_get());
    ImGui::Text("Eating Speed: %0.2fx", character.eatSpeedMultiplier);

    ImGui::SeparatorText("Totals");

    ImGui::Text("Total Calories Consumed: %0.0f", character.totalCaloriesConsumed);

    ImGui::Text("Total Weight Gained: %0.2f %s",
                system == MeasurementSystem::IMPERIAL ? character.totalWeightGained * KG_TO_LB
                                                      : character.totalWeightGained,
                weightUnit);
    ImGui::Text("Food Items Eaten: %i", character.foodItemsEaten);

    ImGui::SeparatorText("Play");

    ImGui::Text("Best: %i pts (%ix)", play.highScore, play.comboBest);
    ImGui::Text("Total Plays: %i", play.totalPlays);

    for (int i = 0; i < Play::GRADE_COUNT; i++)
    {
      auto& value = play.gradeCounts[(Play::Grade)i];
      auto string = std::format("{}", Play::GRADE_STATS_STRINGS[i]);
      ImGui::Text("%s: %i", string.c_str(), value);
    }

    ImGui::Text("Score: %0.2f%%", play.accuracy_score_get());
  }
}
