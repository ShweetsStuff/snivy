#include "imgui.hpp"

namespace game::util::imgui
{
  float row_widget_width_get(int count, float width)
  {
    return (width - (ImGui::GetStyle().ItemSpacing.x * (float)(count - 1))) / (float)count;
  }

  ImVec2 row_widget_size_get(int count, float width) { return ImVec2(row_widget_width_get(count, width), 0); }

  float footer_height_get(int itemCount)
  {
    return ImGui::GetTextLineHeightWithSpacing() * itemCount + ImGui::GetStyle().WindowPadding.y +
           ImGui::GetStyle().ItemSpacing.y * (itemCount);
  }

  ImVec2 footer_size_get(int itemCount)
  {
    return ImVec2(ImGui::GetContentRegionAvail().x, footer_height_get(itemCount));
  }

  ImVec2 size_without_footer_get(int rowCount)
  {
    return ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - footer_height_get(rowCount));
  }
}
