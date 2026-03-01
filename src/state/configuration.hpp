#pragma once

#include "../resources.hpp"

namespace game::state
{
  class Configuration
  {
  public:
    enum Mode
    {
      SELECT,
      MAIN
    };

    bool isGoToSelect{};
    bool isSave{};

    void update(Resources&, Mode = SELECT);
  };
}