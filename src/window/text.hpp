#pragma once

#include "../character.h"
#include "../resources.h"
#include "imgui.h"

namespace game::window
{
  class Text
  {
    resource::Dialogue::Entry* entry{};
    int index{};
    bool isFinished{};

  public:
    resource::Dialogue::Entry::Flag flag{};

    bool isFlagActivated{};

    void set(resource::Dialogue::Entry*, Character&);
    void set_random(std::vector<int>&, Resources&, Character&);
    void tick(Resources&, Character&);
    void update(Resources&, Character&, ImVec2, ImVec2);
  };
}