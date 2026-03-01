#include "widget.hpp"

#include <cmath>

using game::resource::xml::SoundEntryCollection;

namespace game::util::imgui::widget
{
  static SoundEntryCollection* hover{};
  static SoundEntryCollection* select{};

  void sounds_set(SoundEntryCollection* _hover, SoundEntryCollection* _select)
  {
    hover = _hover;
    select = _select;
  }

  void fx()
  {
    static constexpr auto FREQUENCY = 10.0f;
    static constexpr auto ALPHA_MIN = 1.0f;
    static constexpr auto ALPHA_MAX = 1.0f;
    static constexpr auto THICKNESS_MIN = 2.0f;
    static constexpr auto THICKNESS_MAX = 3.0f;

    if (ImGui::IsItemActivated())
      if (select && !select->empty()) select->play();

    if (ImGui::IsItemHovered())
    {
      auto storage = ImGui::GetStateStorage();
      auto id = ImGui::GetItemID();
      if (id == 0) return;
      bool wasHovered = storage->GetBool(id, false);

      if (!wasHovered)
      {
        if (hover && !hover->empty()) hover->play();
        storage->SetBool(id, true);
      }

      auto& style = ImGui::GetStyle();
      auto min = ImGui::GetItemRectMin();
      auto max = ImGui::GetItemRectMax();
      auto time = ImGui::GetTime();
      auto period = sinf((float)(time * FREQUENCY));
      auto thickness = THICKNESS_MIN + (THICKNESS_MAX * period);
      auto colorBorder = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark);
      colorBorder.w = ALPHA_MIN + (ALPHA_MAX * period);

      ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::GetColorU32(colorBorder), style.FrameRounding, 0, thickness);
    }
    else
    {
      auto storage = ImGui::GetStateStorage();
      auto id = ImGui::GetItemID();
      if (id == 0) return;
      storage->SetBool(id, false);
    }
  }
}
