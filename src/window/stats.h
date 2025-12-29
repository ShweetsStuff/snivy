#pragma once

#include "../character.h"
#include "../game_data.h"
#include "../resources.h"

#include "play.h"

#include <imgui.h>

namespace game::window
{
  class Stats
  {
  public:
    void update(Resources&, GameData&, Play&, Character&);
  };
}
