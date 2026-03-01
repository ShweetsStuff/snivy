#pragma once

#include <imgui.h>

#include <glm/glm.hpp>

using namespace glm;

namespace game::util::imgui
{
  float row_widget_width_get(int = 1, float = ImGui::GetContentRegionAvail().x);
  ImVec2 row_widget_size_get(int = 1, float = ImGui::GetContentRegionAvail().x);

  float footer_height_get(int = 1);
  ImVec2 footer_size_get(int = 1);
  ImVec2 size_without_footer_get(int = 1);

  inline ImVec2 to_imvec2(vec2 value) { return ImVec2(value.x, value.y); }
  inline ImVec2 to_imvec2(ivec2 value) { return ImVec2((float)value.x, (float)value.y); }
  inline vec2 to_vec2(ImVec2 value) { return vec2(value.x, value.y); }
  inline ivec2 to_ivec2(ImVec2 value) { return ivec2(value.x, value.y); }

  inline ImVec4 to_imvec4(vec4 value) { return ImVec4(value.r, value.g, value.b, value.a); }
  inline vec4 to_vec4(ImVec4 value) { return vec4(value.x, value.y, value.z, value.w); }
}
