#include "imgui_.h"

namespace game::util::imgui
{
  float row_widget_width_get(int count, float width)
  {
    return (width - (ImGui::GetStyle().ItemSpacing.x * (float)(count - 1))) / (float)count;
  }

  ImVec2 widget_size_with_row_get(int count, float width) { return ImVec2(row_widget_width_get(count, width), 0); }
}
