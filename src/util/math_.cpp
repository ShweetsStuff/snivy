#include "math_.h"
#include "SDL3/SDL_rect.h"
#include "glm/ext/matrix_transform.hpp"
#include <ctime>

using namespace glm;

namespace game::util::math
{
  mat4 quad_model_get(vec2 size, vec2 position, vec2 pivot, vec2 scale, float rotation)
  {
    vec2 scaleAbsolute = glm::abs(scale);
    vec2 scaleSign = glm::sign(scale);
    vec2 pivotScaled = pivot * scaleAbsolute;
    vec2 sizeScaled = size * scaleAbsolute;
    float handedness = (scaleSign.x * scaleSign.y) < 0.0f ? -1.0f : 1.0f;

    mat4 model(1.0f);
    model = glm::translate(model, vec3(position - pivotScaled, 0.0f));
    model = glm::translate(model, vec3(pivotScaled, 0.0f));
    model = glm::scale(model, vec3(scaleSign, 1.0f));
    model = glm::rotate(model, glm::radians(rotation) * handedness, vec3(0, 0, 1));
    model = glm::translate(model, vec3(-pivotScaled, 0.0f));
    model = glm::scale(model, vec3(sizeScaled, 1.0f));
    return model;
  }

  //TODO: Possibly redundant and equivalent to quad_model_get() with a size of (1, 1)?
  mat4 quad_model_parent_get(vec2 position, vec2 pivot, vec2 scale, float rotation)
  {
    vec2 scaleAbsolute = glm::abs(scale);
    vec2 scaleSign = glm::sign(scale);
    float handedness = (scaleSign.x * scaleSign.y) < 0.0f ? -1.0f : 1.0f;

    mat4 local(1.0f);
    local = glm::translate(local, vec3(pivot, 0.0f));
    local = glm::scale(local, vec3(scaleSign, 1.0f));
    local = glm::rotate(local, glm::radians(rotation) * handedness, vec3(0, 0, 1));
    local = glm::translate(local, vec3(-pivot, 0.0f));
    local = glm::scale(local, vec3(scaleAbsolute, 1.0f));

    return glm::translate(mat4(1.0f), vec3(position, 0.0f)) * local;
  }

  bool is_point_in_rect(ivec4 rect, ivec2 point) { return SDL_PointInRect((SDL_Point*)&point, (SDL_Rect*)&rect); }
  bool is_point_in_rectf(vec4 rect, vec2 point) { return SDL_PointInRectFloat((SDL_FPoint*)&point, (SDL_FRect*)&rect); }

  float random() { return (float)rand() / RAND_MAX; }
  bool random_percent_roll(float percent) { return to_percent(random()) < percent; }
  float random_in_range(float min, float max) { return min + random() * (max - min); }
  float random_roll(float value) { return random() * value; }
  bool random_bool() { return random() < 0.5f; };
  void random_seed_set() { srand(std::time(nullptr)); }
}