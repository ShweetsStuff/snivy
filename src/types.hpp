#pragma once

#include "glm/ext/vector_float4.hpp"

namespace game
{
  enum MeasurementSystem
  {
    METRIC,
    IMPERIAL
  };

  constexpr auto KG_TO_LB = 2.20462;
  constexpr auto WHITE = glm::vec4();
  constexpr auto GRAY = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
}
