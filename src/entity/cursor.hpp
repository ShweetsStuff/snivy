#pragma once

#include "../util/interact_type.hpp"
#include "actor.hpp"

namespace game::entity
{
  class Cursor : public Actor
  {
  public:
    enum State
    {
      DEFAULT,
      HOVER,
      ACTION
    };

    State state{DEFAULT};
    InteractType mode{InteractType::RUB};

    Cursor() = default;
    Cursor(resource::xml::Anm2&);
    void tick();
    void update();
  };
}