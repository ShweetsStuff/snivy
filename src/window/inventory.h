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
  private:
    std::map<Item::Type, int> values = {{Item::POKE_PUFF_BASIC_SWEET, 1},
                                        {Item::POKE_PUFF_BASIC_CITRUS, 1},
                                        {Item::POKE_PUFF_BASIC_MINT, 1},
                                        {Item::POKE_PUFF_BASIC_MOCHA, 1},
                                        {Item::POKE_PUFF_BASIC_SPICE, 1}};

  public:
    static constexpr auto ITEM_SIZE = glm::vec2(48, 48);
    static constexpr auto IMAGE_SIZE = ImVec2(48, 48);
    static constexpr auto BUTTON_ROUNDING = 32.0f;
    int count;

    bool isQueued{};
    Item::Type queuedItemType{};

    Inventory();
    void update(Resources&, Character&, GameData&);
    void adjust_item(Item::Type, int = 1);
    void set_item(Item::Type, int);
    int get_item(Item::Type);
  };
}
