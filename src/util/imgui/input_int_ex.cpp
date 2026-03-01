#include "input_int_ex.hpp"

#include <algorithm>

namespace game::util::imgui
{
  bool input_int_range(const char* label, int* value, int min, int max, int step, int stepFast,
                       ImGuiInputTextFlags flags)
  {
    auto isChanged = ImGui::InputInt(label, value, step, stepFast, flags);

    auto minMax = std::minmax(min, max);
    auto clamped = std::clamp(*value, minMax.first, minMax.second);

    if (clamped != *value)
    {
      *value = clamped;
      isChanged = true;
    }

    return isChanged;
  }
}
