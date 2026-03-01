#include "state.hpp"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

#include "util/math.hpp"

using namespace glm;
using namespace game::util;
using namespace game::state;

namespace game
{
  constexpr auto TICK_RATE = 30;
  constexpr auto TICK_INTERVAL = (1000 / TICK_RATE);
  constexpr auto UPDATE_RATE = 60;
  constexpr auto UPDATE_INTERVAL = (1000 / UPDATE_RATE);

  State::State(SDL_Window* _window, SDL_GLContext _context, resource::xml::Settings settings)
      : window(_window), context(_context), canvas(settings.windowSize, Canvas::DEFAULT)
  {
    resources.settings = settings;
    SDL_SetWindowSize(window, resources.settings.windowSize.x, resources.settings.windowSize.y);
  }

  void State::tick()
  {
    switch (type)
    {
      case SELECT:
        select.tick();
        break;
      case MAIN:
        main.tick(resources);
        break;
      default:
        break;
    }
  }

  void State::update()
  {
#ifndef __EMSCRIPTEN__
    SDL_GetWindowSize(window, &resources.settings.windowSize.x, &resources.settings.windowSize.y);
#endif

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT)
      {
        if (type == MAIN) main.exit(resources);
        isRunning = false;
      }
      if (!isRunning) return;
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();

    switch (type)
    {
      case SELECT:
        select.update(resources);
        if (select.info.isNewGame || select.info.isContinue)
        {
          Main::Game game = select.info.isNewGame ? Main::NEW_GAME : Main::CONTINUE;
          if (game == Main::NEW_GAME) resources.character_save_set(select.characterIndex, resource::xml::Save());

          main.set(resources, select.characterIndex, game);
          type = MAIN;

          select.info.isNewGame = false;
          select.info.isContinue = false;
        }
        break;
      case MAIN:
        main.update(resources);
        if (main.menu.configuration.isGoToSelect)
        {
          main.exit(resources);
          type = SELECT;
          main.menu.configuration.isGoToSelect = false;
        }
        break;
      default:
        break;
    }

    auto isHideCursor = type == MAIN;
    if (isHideCursor != isCursorHidden)
    {
      if (isHideCursor)
        SDL_HideCursor();
      else
        SDL_ShowCursor();
      isCursorHidden = isHideCursor;
    }
  }

  void State::render()
  {
    auto windowSize = resources.settings.windowSize;
#ifndef __EMSCRIPTEN__
    SDL_GetWindowSize(window, &windowSize.x, &windowSize.y);
#endif

    canvas.bind();
    canvas.size_set(windowSize);
    canvas.clear(vec4(resources.settings.color, 1.0f));
    canvas.unbind();

    switch (type)
    {
      case SELECT:
        select.render(resources, canvas);
        break;
      case MAIN:
        main.render(resources, canvas);
        break;
      default:
        break;
    }

    SDL_GL_SwapWindow(window);
  }

  void State::loop()
  {
    auto currentTick = SDL_GetTicks();
    auto currentUpdate = SDL_GetTicks();

    if (currentUpdate - previousUpdate >= UPDATE_INTERVAL)
    {
      update();
      render();
      previousUpdate = currentUpdate;
    }

    if (currentTick - previousTick >= TICK_INTERVAL)
    {
      tick();
      previousTick = currentTick;
    }

    SDL_Delay(1);
  }
}
