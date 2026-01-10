#pragma once

#include "../character.h"
#include "../game_data.h"
#include "../resources.h"

#include <imgui.h>

namespace game::window
{
  class Info
  {
  public:
    void update(Resources&, GameData&, Character&, ImVec2, ImVec2);
  };
}
