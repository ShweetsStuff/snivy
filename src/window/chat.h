#pragma once

#include "../character.h"
#include "../game_data.h"
#include "../resources.h"

#include "text.h"

#include <imgui.h>

namespace game::window
{
  class Chat
  {
  public:
    void update(Resources&, GameData&, Text&, Character&);
  };
}
