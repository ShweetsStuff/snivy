#pragma once

#include <imgui.h>

namespace game::util::imgui
{
  bool input_int_range(const char* label, int* value, int min, int max, int step = 1, int stepFast = 100,
                       ImGuiInputTextFlags flags = 0);
}
