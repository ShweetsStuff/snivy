#pragma once

#include "resource/xml/settings.hpp"
#include <SDL3/SDL.h>
#include <glm/ext/vector_float2.hpp>

namespace game
{
  class Loader
  {
  public:
    SDL_Window* window{};
    SDL_GLContext context{};
    bool isError{};
    resource::xml::Settings settings;

    Loader(int argc, const char** argv);
    ~Loader();
  };

};
