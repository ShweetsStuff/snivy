#include "stats.hpp"

#include <ranges>

#include "../../util/measurement.hpp"

using namespace game::resource;
using namespace game::util;

namespace game::state::main
{
  void Stats::update(Resources& resources, Play& play, entity::Character& character)
  {
    ImGui::PushFont(ImGui::GetFont(), Font::BIG);
    ImGui::TextUnformatted(character.data.name.c_str());
    ImGui::PopFont();

    ImGui::Separator();

    auto& playSchema = character.data.playSchema;
    auto& system = resources.settings.measurementSystem;
    auto weight = character.weight_get(system);
    auto weightUnit = system == measurement::IMPERIAL ? "lbs" : "kg";

    ImGui::Text("Weight: %0.2f %s (Stage: %i)", weight, weightUnit, character.stage_get() + 1);
    ImGui::Text("Capacity: %0.0f kcal (Max: %0.0f kcal)", character.capacity, character.max_capacity());
    ImGui::Text("Digestion Rate: %0.2f%%/sec", character.digestion_rate_get());
    ImGui::Text("Eating Speed: %0.2fx", character.eatSpeed);

    ImGui::SeparatorText("Totals");

    ImGui::Text("Total Calories Consumed: %0.0f kcal", character.totalCaloriesConsumed);
    ImGui::Text("Total Food Items Eaten: %i", character.totalFoodItemsEaten);

    ImGui::SeparatorText("Play");

    ImGui::Text("Best: %i pts (%ix)", play.highScore, play.bestCombo);
    ImGui::Text("Total Plays: %i", play.totalPlays);

    for (int i = 0; i < (int)playSchema.grades.size(); i++)
    {
      auto& grade = playSchema.grades[i];
      ImGui::Text("%s: %i", grade.namePlural.c_str(), play.gradeCounts[i]);
    }

    ImGui::Text("Accuracy: %0.2f%%", play.accuracy_score_get(character));
  }
}
