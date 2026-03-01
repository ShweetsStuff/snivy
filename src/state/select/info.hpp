#pragma once

#include "../../resources.hpp"

namespace game::state::select
{
  class Info
  {
  public:
    bool isContinue{};
    bool isNewGame{};
    bool isNewGameWarning{};

    void update(Resources&, int characterIndex);
  };
}