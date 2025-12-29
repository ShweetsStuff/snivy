#pragma once

#include <imgui.h>

#include <glm/glm.hpp>

using namespace glm;

namespace game::util::imgui
{
  float widget_width_with_row_get(int = 1, float = ImGui::GetContentRegionAvail().x);
  ImVec2 widget_size_with_row_get(int = 1, float = ImGui::GetContentRegionAvail().x);

  inline ImVec2 to_imvec2(vec2 value) { return ImVec2(value.x, value.y); }
  inline vec2 to_vec2(ImVec2 value) { return vec2(value.x, value.y); }

  inline ImVec4 to_imvec4(vec4 value) { return ImVec4(value.r, value.g, value.b, value.a); }
  inline vec4 to_vec4(ImVec4 value) { return vec4(value.x, value.y, value.z, value.w); }
}