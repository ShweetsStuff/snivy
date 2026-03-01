#pragma once

#include "../../entity/character.hpp"
#include "../../resources.hpp"

#include <imgui.h>

namespace game::state::main
{
  class Info
  {
  public:
    void update(Resources&, entity::Character&);
  };
}
