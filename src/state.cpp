#include "state.h"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

using namespace glm;
using namespace game::resource;

namespace game
{
  constexpr auto TICK_RATE = 30;
  constexpr auto TICK_INTERVAL = (1000 / TICK_RATE);
  constexpr auto UPDATE_RATE = 120;
  constexpr auto UPDATE_INTERVAL = (1000 / UPDATE_RATE);

  State::State(SDL_Window* inWindow, SDL_GLContext inContext, vec2 size)
      : window(inWindow), context(inContext), canvas(size, true)
  {
  }

  void State::tick() { actor.tick(); }

  void State::update()
  {
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT) isRunning = false;
    }
    if (!isRunning) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Metrics");
    ImGui::Text("Time: %f", actor.time);
    ImGui::Text("IS Playing: %s", actor.isPlaying ? "true" : "false");

    auto animation = actor.animation_get();
    ImGui::Text("Animation: %s", animation ? animation->name.c_str() : "null");
    ImGui::End();

    ImGui::Render();
  }

  void State::render()
  {
    SDL_GL_MakeCurrent(window, context);

    int width{};
    int height{};
    SDL_GetWindowSize(window, &width, &height);

    canvas.bind();
    canvas.clear(glm::vec4(0, 0, 0, 1));
    actor.render(resources.shaders[shader::TEXTURE], canvas);
    canvas.unbind();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
