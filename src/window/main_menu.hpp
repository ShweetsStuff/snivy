#pragma once

#include <imgui.h>

#include "chat.h"
#include "play.h"
#include "stats.h"
#include "text.h"

namespace game::window
{
  class MainMenu
  {
  public:
    Play play;
    Chat chat;
    Stats stats;
    Inventory inventory;

    bool isCheats{};

    void update(Resources&, Character&, GameData&, Text& text, ImVec2 size, ImVec2 pos);
  };
}
