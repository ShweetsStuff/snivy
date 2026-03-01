#pragma once

#include "../../entity/character.hpp"

#include "../../resources.hpp"

#include "item_manager.hpp"

#include <imgui.h>

namespace game::state::main
{
  class Inventory
  {
  public:
    static constexpr auto SIZE = 96.0f;

    std::map<int, int> values{};
    std::unordered_map<int, entity::Actor> actors{};
    std::unordered_map<int, glm::vec4> rects{};
    std::unordered_map<int, Canvas> canvases{};

    void tick();
    void update(Resources&, ItemManager&, entity::Character&);
    int count();
  };
}
