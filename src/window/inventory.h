#pragma once

#include "../character.h"
#include "../game_data.h"
#include "../item.h"
#include "../resources.h"

#include <imgui.h>

namespace game::window
{
  class Inventory
  {
  public:
    static constexpr auto ITEM_SIZE = glm::vec2(48, 48);
    static constexpr auto IMAGE_SIZE = ImVec2(48, 48);
    static constexpr auto BUTTON_ROUNDING = 32.0f;

    std::map<Item::Type, int> values = {{Item::POKE_PUFF_BASIC_SWEET, 1},
                                        {Item::POKE_PUFF_BASIC_CITRUS, 1},
                                        {Item::POKE_PUFF_BASIC_MINT, 1},
                                        {Item::POKE_PUFF_BASIC_MOCHA, 1},
                                        {Item::POKE_PUFF_BASIC_SPICE, 1}};

    bool isQueued{};
    Item::Type queuedItemType{};

    void update(Resources&, Character&, GameData&);
  };
}
