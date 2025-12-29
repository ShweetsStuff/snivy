#include "main_menu.h"

namespace game::window
{
  void MainMenu::update(Resources& resources, Character& character, GameData& gameData, Text& text, ImVec2 size,
                        ImVec2 pos)
  {
    MeasurementSystem& measurementSystem = gameData.measurementSystem;

    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos(pos);

    if (ImGui::Begin("##Main", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove))
    {

      if (ImGui::BeginTabBar("##Options", ImGuiTabBarFlags_FittingPolicyResizeDown))
      {

        if (ImGui::BeginTabItem("Chat"))
        {
          chat.update(resources, gameData, text, character);
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Play"))
        {
          play.update(resources, character, inventory, gameData, text);
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Inventory"))
        {
          inventory.update(resources, character, gameData);
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Stats"))
        {
          stats.update(resources, gameData, play, character);
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Settings"))
        {
          ImGui::SeparatorText("Measurement System");
          ImGui::RadioButton("Metric", (int*)&measurementSystem, MeasurementSystem::METRIC);
          ImGui::SameLine();
          ImGui::RadioButton("Imperial", (int*)&measurementSystem, MeasurementSystem::IMPERIAL);
          ImGui::EndTabItem();
        }

        if (isCheats && ImGui::BeginTabItem("Cheats"))
        {
          if (ImGui::Button("Feed"))
            character.calories = std::min(character.calories + 100.0f, character.max_capacity());
          ImGui::SameLine();
          if (ImGui::Button("Starve")) character.calories = std::max(0.0f, character.calories - 100.0f);
          ImGui::SameLine();
          if (ImGui::Button("Digest"))
          {
            character.digestionProgress = Character::DIGESTION_MAX;
            if (character.calories == 0.0f) character.calories = 0.001f;
          }
          ImGui::SameLine();
          ImGui::Checkbox("Show Nulls (Hitboxes)", &character.isShowNulls);

          if (ImGui::DragInt("Stage", &character.weightStage, 0.1f, 0, Character::WEIGHT_STAGE_MAX - 1))
          {
            character.weight = Character::WEIGHT_THRESHOLDS[character.weightStage];
            character.weight = character.weight < character.highestWeight ? character.highestWeight : character.weight;
            character.state_set(Character::IDLE, true);
            character.isForceStageUp = true;
          }

          ImGui::DragFloat("Digestion Rate", &character.digestionRate, 0.005f, Character::DIGESTION_RATE_MIN,
                           Character::DIGESTION_RATE_MAX);
          ImGui::DragFloat("Eat Speed", &character.eatSpeedMultiplier, 0.1f, Character::EAT_SPEED_MULTIPLIER_MIN,
                           Character::EAT_SPEED_MULTIPLIER_MAX);

          ImGui::SeparatorText("Animations");
          ImGui::Text("Now Playing: %s", character.anm2->animations.mapReverse.at(character.animationIndex).c_str());

          if (ImGui::BeginChild("## Animations", {0, 100}, ImGuiChildFlags_Borders))
          {
            for (int i = 0; i < character.anm2->animations.items.size(); i++)
            {
              auto& animation = character.anm2->animations.items[i];
              ImGui::PushID(i);
              if (ImGui::Selectable(animation.name.c_str()))
                character.play(animation.name.c_str(), Character::FORCE_PLAY);
              ImGui::PopID();
            }
          }
          ImGui::EndChild();

          ImGui::SeparatorText("Dialogue");

          if (ImGui::BeginChild("## Dialogue", {0, 100}, ImGuiChildFlags_Borders))
          {
            for (auto& [label, i] : resources.dialogue.labelMap)
            {
              ImGui::PushID(i);
              if (ImGui::Selectable(label.c_str())) text.set(&resources.dialogue.entryMap.at(i), character);
              ImGui::PopID();
            }
          }
          ImGui::EndChild();

          ImGui::SeparatorText("Inventory");

          if (ImGui::BeginChild("## Inventory", ImGui::GetContentRegionAvail(), ImGuiChildFlags_Borders))
          {
            ImGui::PushItemWidth(100);
            for (int i = 0; i < Item::ITEM_COUNT; i++)
            {
              if (i == Item::INVALID) continue;
              ImGui::PushID(i);
              ImGui::DragInt(Item::NAMES[i], &inventory.values[(Item::Type)i], 0.1f, 0, 999);
              ImGui::PopID();
            }
            ImGui::PopItemWidth();
          }
          ImGui::EndChild();

          ImGui::EndTabItem();
        }
      }

      ImGui::EndTabBar();
    }
    ImGui::End();
  }
}