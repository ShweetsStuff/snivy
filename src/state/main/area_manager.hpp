#pragma once

#include "../../entity/character.hpp"

namespace game::state::main
{
  class AreaManager
  {
  public:
    int get(entity::Character&);
  };
}
