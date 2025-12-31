#include "inventory.h"

#include "../util/imgui_.h"
#include <format>

using namespace game::util;
using namespace game::resource;
using namespace glm;

namespace game::window
{
  Inventory::Inventory()
  {
    count = 0;

    for (auto& [type, quantity] : values)
    {
      count += quantity;
    }
  }

  void Inventory::adjust_item(Item::Type type, int quantity)
  {
    values[type] += quantity;
    count += quantity;
  }

  void Inventory::set_item(Item::Type type, int value)
  {
    count -= values[type];
    values[type] = value;
    count += value;
  }

  int Inventory::get_item(Item::Type type) { return values.contains(type) ? values[type] : 0; }

  void Inventory::update(Resources& resources, Character& character, GameData& gameData)
  {
    auto& texture = resources.anm2s[anm2::ITEMS].content.spritesheets.at(0).texture;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, BUTTON_ROUNDING);

    auto cursorPos = ImGui::GetCursorPos();
    auto cursorStartX = ImGui::GetCursorPosX();

    for (int index = 0; index < Item::ITEM_COUNT; index++)
    {
      auto type = (Item::Type)index;
      auto seen = values.contains(type);

      //Hide invalid items and impossible-to-obtain items when not seen.
      if (!seen && (Item::CATEGORIES[type] == Item::INVALID || Item::RARITIES[type] == Item::IMPOSSIBLE)) continue;

      auto columns = (int)(texture.size.x / ITEM_SIZE.x);
      auto crop = vec2(type % columns, type / columns) * ITEM_SIZE;
      auto uvMin = crop / vec2(texture.size);
      auto uvMax = (crop + ITEM_SIZE) / vec2(texture.size);

      ImGui::PushID(type);
      ImGui::SetCursorPos(cursorPos);
      auto cursorScreenPos = ImGui::GetCursorScreenPos();

      auto quantity = 0;

      if (seen)
      {
        quantity = values[type];

        ImGui::BeginDisabled(quantity < 1);
        if (ImGui::ImageButton("##Image Button", texture.id, IMAGE_SIZE, imgui::to_imvec2(uvMin),
                               imgui::to_imvec2(uvMax)) &&
            quantity > 0)
        {
          queuedItemType = type;
          isQueued = true;

          resources.sound_play(audio::SUMMON);
        }
        ImGui::EndDisabled();

        if (ImGui::BeginItemTooltip())
        {
          auto& category = Item::CATEGORIES[type];
          auto& rarity = Item::RARITIES[type];
          auto& flavor = Item::FLAVORS[type];
          auto& calories = Item::CALORIES[type];
          auto& digestionRateBonus = Item::DIGESTION_RATE_BONUSES[type];
          auto& eatSpeedBonus = Item::EAT_SPEED_BONUSES[type];

          ImGui::Text("%s (x%i)", Item::NAMES[type], quantity);

          ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(imgui::to_imvec4(GRAY)));
          ImGui::Text("-- %s (%s) --", Item::CATEGORY_NAMES[category], Item::RARITY_NAMES[rarity]);
          if (category == Item::FOOD)
          {
            ImGui::Separator();
            if (flavor != Item::FLAVORLESS) ImGui::Text("Flavor: %s", Item::FLAVOR_NAMES[flavor]);
            if (calories != 0) ImGui::Text("%0.0f kcal", calories);
            if (digestionRateBonus > 0)
              ImGui::Text("Digestion Rate Bonus: +%0.2f%% / sec", digestionRateBonus * 60.0f);
            else if (digestionRateBonus < 0)
              ImGui::Text("Digestion Rate Penalty: %0.2f%% / sec", digestionRateBonus * 60.0f);
            if (eatSpeedBonus > 0) ImGui::Text("Eat Speed Bonus: +%0.2fx ", eatSpeedBonus);
          }
          ImGui::Separator();
          ImGui::TextUnformatted(Item::DESCRIPTIONS[type]);
          ImGui::PopStyleColor();
          ImGui::EndTooltip();
        }

        ImGui::PushFont(resources.font.get(), Font::BIG);
        auto text = std::format("x{}", quantity);
        auto textPos = ImVec2(cursorScreenPos.x + IMAGE_SIZE.x - ImGui::CalcTextSize(text.c_str()).x,
                              cursorScreenPos.y + IMAGE_SIZE.y - ImGui::GetTextLineHeight());
        ImGui::GetWindowDrawList()->AddText(textPos, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)),
                                            text.c_str());
        ImGui::PopFont();
      }
      else
      {
        ImGui::BeginDisabled(true);
        ImGui::ImageButton("##Image Button", texture.id, IMAGE_SIZE, imgui::to_imvec2(uvMin), imgui::to_imvec2(uvMax),
                           ImVec4(0, 0, 0, 0), ImVec4(0, 0, 0, 1));
        ImGui::EndDisabled();

        if (ImGui::BeginItemTooltip())
        {
          ImGui::TextUnformatted("??? (x0)");

          ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(imgui::to_imvec4(GRAY)));
          ImGui::TextUnformatted("-- ??? (\?\?\?) --");

          ImGui::Separator();
          ImGui::TextUnformatted("???");

          ImGui::PopStyleColor();
          ImGui::EndTooltip();
        }
      }

      auto increment = ImGui::GetItemRectSize().x + ImGui::GetStyle().ItemSpacing.x;
      cursorPos.x += increment;

      if (cursorPos.x + increment > ImGui::GetContentRegionAvail().x)
      {
        cursorPos.x = cursorStartX;
        cursorPos.y += increment;
      }

      ImGui::PopID();
    }

    if (count == 0) ImGui::Text("Check the \"Play\" tab to earn rewards!");

    ImGui::PopStyleVar();
  }
}
