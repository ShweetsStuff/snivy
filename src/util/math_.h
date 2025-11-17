#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"

namespace game::util::math
{
  glm::mat4 quad_model_get(glm::vec2, glm::vec2, glm::vec2, glm::vec2, float);
  glm::mat4 quad_model_parent_get(glm::vec2 position, glm::vec2 pivot, glm::vec2, float);

  template <typename T> constexpr T percent_to_unit(T value) { return value / 100.0f; }
  template <typename T> constexpr T unit_to_percent(T value) { return value * 100.0f; }

  constexpr std::array<float, 16> uv_vertices_get(glm::vec2 uvMin, glm::vec2 uvMax)
  {
    return {0.0f, 0.0f, uvMin.x, uvMin.y, 1.0f, 0.0f, uvMax.x, uvMin.y,
            1.0f, 1.0f, uvMax.x, uvMax.y, 0.0f, 1.0f, uvMin.x, uvMax.y};
  }
}