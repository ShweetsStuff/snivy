#pragma once

#include "../../resources.hpp"
#include "../configuration.hpp"

namespace game::state::select
{
  class Characters
  {
  public:
    Configuration configuration;

    void update(Resources&, int& characterIndex);
  };
}