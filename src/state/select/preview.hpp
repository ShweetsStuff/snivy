#pragma once

#include "../../entity/actor.hpp"
#include "../../resources.hpp"

namespace game::state::select
{
  class Preview
  {
  public:
    int previousCharacterIndex{-1};
    entity::Actor actor{};
    glm::vec4 rect{};
    bool isInGame{};

    Canvas canvas{glm::vec2(), Canvas::FLIP};

    void update(Resources& resources, int characterIndex);
    void tick(int characterIndex);
  };
}
