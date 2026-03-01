#include "math.hpp"
#include "SDL3/SDL_rect.h"
#include "glm/ext/matrix_transform.hpp"
#include <ctime>

using namespace glm;

namespace game::util::math
{
  static mat4 quad_model_local_get(vec2 pivot, vec2 scale, float rotation)
  {
    vec2 scaleSign = glm::sign(scale);
    vec2 scaleAbsolute = glm::abs(scale);
    float handedness = (scaleSign.x * scaleSign.y) < 0.0f ? -1.0f : 1.0f;

    mat4 local(1.0f);
    local = glm::translate(local, vec3(pivot, 0.0f));
    local = glm::scale(local, vec3(scaleSign, 1.0f));
    local = glm::rotate(local, glm::radians(rotation) * handedness, vec3(0, 0, 1));
    local = glm::translate(local, vec3(-pivot, 0.0f));
    local = glm::scale(local, vec3(scaleAbsolute, 1.0f));
    return local;
  }

  mat4 quad_model_get(vec2 size, vec2 position, vec2 pivot, vec2 scale, float rotation)
  {
    vec2 scaleAbsolute = glm::abs(scale);
    vec2 scaleSign = glm::sign(scale);
    vec2 pivotScaled = pivot * scaleAbsolute;
    vec2 sizeScaled = size * scaleAbsolute;

    return glm::translate(mat4(1.0f), vec3(position - pivotScaled, 0.0f)) *
           quad_model_local_get(pivotScaled, scaleSign, rotation) * glm::scale(mat4(1.0f), vec3(sizeScaled, 1.0f));
  }

  mat4 quad_model_no_size_get(vec2 position, vec2 pivot, vec2 scale, float rotation)
  {
    return glm::translate(mat4(1.0f), vec3(position, 0.0f)) * quad_model_local_get(pivot, scale, rotation);
  }

  bool is_point_in_rect(ivec4 rect, ivec2 point) { return SDL_PointInRect((SDL_Point*)&point, (SDL_Rect*)&rect); }
  bool is_point_in_rectf(vec4 rect, vec2 point) { return SDL_PointInRectFloat((SDL_FPoint*)&point, (SDL_FRect*)&rect); }

  float random() { return (float)rand() / (float)RAND_MAX; }
  bool random_percent_roll(float percent) { return to_percent(random()) < percent; }
  float random_in_range(float min, float max) { return min + random() * (max - min); }
  float random_max(float max) { return random_in_range(0, max); }
  float random_roll(float value) { return random() * value; }
  bool random_bool() { return random() < 0.5f; };
  void random_seed_set() { srand((unsigned int)std::time(nullptr)); }
}
