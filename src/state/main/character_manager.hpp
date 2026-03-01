#pragma once

#include "../../entity/character.hpp"
#include "../../entity/cursor.hpp"
#include "text.hpp"

namespace game::state::main
{
  class CharacterManager
  {
  public:
    bool isInteracting{};
    bool isHovering{};
    bool isInteractingPrevious{};
    bool isHoveringPrevious{};
    bool isJustStoppedInteracting{};
    bool isJustStoppedHovering{};
    int interactAreaID{-1};
    InteractType lastInteractType{(InteractType)-1};

    glm::vec2 cursorWorldPositionPrevious{};
    std::string queuedAnimation{};

    void update(entity::Character&, entity::Cursor&, Text&, Canvas&);
  };
}
