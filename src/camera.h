#pragma once

#include <SDL3/SDL.h>

#include "loader.h"

#include "resources.h"
#include "types.h"
#include "util/math_.h"

namespace game
{

  class Camera
  {
    glm::vec2 targetSize{Loader::SIZE};
    glm::vec2 actualSize{Loader::SIZE};

    glm::vec2 cameraPosition{glm::vec2(0, 0)};
    glm::vec2 cropOffset{glm::vec2(0, 0)};

    float cropScale{1.0f};

  public:
    void on_resize(int width, int height);
    void update();
    glm::mat4 get_model(glm::vec2 size, glm::vec2 position = {}, glm::vec2 pivot = {},
                        glm::vec2 scale = glm::vec2(1.0f), float rotation = {});

    glm::mat4 get_root_model(glm::vec2 position = {}, glm::vec2 pivot = {}, glm::vec2 scale = glm::vec2(1.0f),
                             float rotation = {});

    glm::ivec4 get_bounds(glm::vec2 position, glm::vec2 size);

    glm::ivec2 to_screen(glm::vec2 world);
    glm::ivec2 to_world(glm::vec2 screen);
  };
};
