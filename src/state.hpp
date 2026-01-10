#pragma once

#include <SDL3/SDL.h>

#include "character.h"
#include "cursor.h"
#include "item.h"

#include "canvas.h"
#include "resources.h"

#include "window/info.h"
#include "window/main_menu.h"
#include "window/text.h"

namespace game
{

  class State
  {
    SDL_Window* window{};
    SDL_GLContext context{};
    long previousUpdate{};
    long previousTick{};

    Resources resources;

    Character character{&resources.anm2s[anm2::CHARACTER], glm::vec2(300, 500)};
    Cursor cursor{&resources.anm2s[anm2::CURSOR]};

    std::vector<Item> items{};

    window::Info infoWindow;
    window::MainMenu mainMenuWindow;
    window::Text textWindow;

    bool isMainMenu{false};
    bool isInfo{false};
    bool isText{false};

    GameData gameData;

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
