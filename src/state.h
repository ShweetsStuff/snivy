#pragma once

#include <SDL3/SDL.h>

#include "resource/actor.h"

#include "canvas.h"
#include "resources.h"

namespace game
{
  class State
  {
    SDL_Window* window{};
    SDL_GLContext context{};

    Resources resources;

    resource::Actor actor{"resources/anm2/snivy.anm2", glm::vec2(400, 400)};

    long previousUpdate{};
    long previousTick{};

    void tick();
    void update();
    void render();

  public:
    bool isRunning{true};
    Canvas canvas{};

    State(SDL_Window*, SDL_GLContext, glm::vec2);
    void loop();
  };
};
