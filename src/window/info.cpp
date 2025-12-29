#include "info.h"

#include "../util/math_.h"
#include "../util/string_.h"

#include <algorithm>
#include <format>

namespace game::window
{

  void Info::update(Resources& resources, GameData& gameData, Character& character, ImVec2 size, ImVec2 pos)
  {
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos(pos);

    if (ImGui::Begin("##Info", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove))
    {
      auto childSize = ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y);

      if (ImGui::BeginChild("##Weight", childSize))
      {
        auto& system = gameData.measurementSystem;
        auto weight = character.weight_get(system);

        ImGui::TextUnformatted(
            std::format("{} {}", util::string::format_commas(weight, 2), system == IMPERIAL ? "lbs" : "kg").c_str());

        if (character.weightStage >= character.WEIGHT_STAGE_MAX - 1)
        {
          ImGui::ProgressBar(1.0f, ImVec2(), "MAX");
          ImGui::SetItemTooltip("Maxed out!");
        }
        else
        {
          auto weightCurrent = character.weight_threshold_current_get(system);
          auto weightNext = character.weight_threshold_next_get(system);
          auto weightProgress = character.progress_to_next_weight_threshold_get();

          ImGui::ProgressBar(weightProgress, ImVec2(), "To Next Stage");
          auto format = system == IMPERIAL ? "Start: %0.2flbs | Current: %0.2flbs | Goal: %0.2flbs | (%0.2f%%)"
                                           : "Start: %0.2fkg | Current: %0.2fkg | Goal: %0.2fkg | (%0.2f%%)";
          ImGui::SetItemTooltip(format, weightCurrent, weight, weightNext, util::math::to_percent(weightProgress));
        }
      }
      ImGui::EndChild();

      ImGui::SameLine();

      if (ImGui::BeginChild("##Calories and Capacity", childSize))
      {
        auto& calories = character.calories;
        auto& capacity = character.capacity;

        auto overstuffedPercent = std::max(0.0f, (calories - capacity) / (character.max_capacity() - capacity));
        auto caloriesColor = ImVec4(1.0f, 1.0f - overstuffedPercent, 1.0f - overstuffedPercent, 1.0f);

        ImGui::PushStyleColor(ImGuiCol_Text, caloriesColor);
        ImGui::Text("%0.0lf kcal", calories);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (character.is_over_capacity()) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
        ImGui::Text("/ %0.0lf kcal", character.is_over_capacity() ? character.max_capacity() : character.capacity);
        if (character.is_over_capacity()) ImGui::PopStyleColor();

        auto digestionProgress = character.isDigesting
                                     ? (float)character.digestionTimer / Character::DIGESTION_TIMER_MAX
                                     : character.digestionProgress / character.DIGESTION_MAX;
        ImGui::ProgressBar(digestionProgress, ImVec2(), character.isDigesting ? "Digesting..." : "Digestion");

        if (ImGui::BeginItemTooltip())
        {
          if (character.isDigesting)
            ImGui::TextUnformatted("Digestion in progress...");
          else if (digestionProgress <= 0.0f)
            ImGui::TextUnformatted("Give food to start digesting!");

          ImGui::Text("%0.2f%%", util::math::to_percent(digestionProgress));

          ImGui::Text("Rate: %0.2f%% / sec", character.digestion_rate_second_get());

          ImGui::Text("Eating Speed: %0.2fx", character.eatSpeedMultiplier);

          ImGui::EndTooltip();
        }
      }
      ImGui::EndChild();
    }
    ImGui::End();
  }
}
