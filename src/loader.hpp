#pragma once

#include "glm/ext/vector_float2.hpp"
#include <SDL3/SDL.h>

namespace game
{
  class Loader
  {
  public:
    static constexpr glm::vec2 SIZE = {1080, 720};

    SDL_Window* window{};
    SDL_GLContext context{};
    bool isError{};

    Loader();
    ~Loader();
  };

};
