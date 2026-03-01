#pragma once

#include "resource/xml/settings.hpp"
#include <SDL3/SDL.h>
#include <glm/ext/vector_float2.hpp>

namespace game
{
  class Loader
  {
  public:
    static constexpr glm::vec2 SIZE = {1280, 720};

    SDL_Window* window{};
    SDL_GLContext context{};
    bool isError{};
    resource::xml::Settings settings;

    Loader(int argc, const char** argv);
    ~Loader();
  };

};
