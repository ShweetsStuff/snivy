#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"

namespace game::util::math
{
  glm::mat4 quad_model_get(glm::vec2 size, glm::vec2 position = {}, glm::vec2 pivot = {},
                           glm::vec2 scale = glm::vec2(1.0f), float rotation = {});
  glm::mat4 quad_model_no_size_get(glm::vec2 position = {}, glm::vec2 pivot = {}, glm::vec2 scale = glm::vec2(1.0f),
                                   float rotation = {});

  template <typename T> constexpr T to_percent(T value) { return value * 100.0f; }
  template <typename T> constexpr T to_unit(T value) { return value / 100.0f; }

  constexpr std::array<float, 16> uv_vertices_get(glm::vec2 uvMin, glm::vec2 uvMax)
  {
    return {0.0f, 0.0f, uvMin.x, uvMin.y, 1.0f, 0.0f, uvMax.x, uvMin.y,
            1.0f, 1.0f, uvMax.x, uvMax.y, 0.0f, 1.0f, uvMin.x, uvMax.y};
  }

  bool is_point_in_rect(glm::ivec4 rect, glm::ivec2 point);
  bool is_point_in_rectf(glm::vec4 rect, glm::vec2 point);

  float random();
  bool random_percent_roll(float percent);
  float random_roll(float value);
  float random_in_range(float min, float max);
  float random_max(float max);
  bool random_bool();
  void random_seed_set();
}
