#pragma once

#include "text.hpp"

#include <imgui.h>

namespace game::state::main
{
  class Chat
  {
  public:
    void update(Resources&, Text&, entity::Character&);
  };
}
