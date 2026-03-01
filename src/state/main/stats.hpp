#pragma once

#include "../../entity/character.hpp"
#include "../../resources.hpp"

#include "play.hpp"

#include <imgui.h>

namespace game::state::main
{
  class Stats
  {
  public:
    void update(Resources&, Play&, entity::Character&);
  };
}
