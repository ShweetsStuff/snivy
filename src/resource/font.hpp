#pragma once

#include "imgui.h"
#include <string>

namespace game::resource
{
  class Font
  {
  public:
    ImFont* internal;

    static constexpr auto NORMAL = 12;
    static constexpr auto BIG = 16;
    static constexpr auto LARGE = 24;

    Font(const std::string&, float = NORMAL);
    ImFont* get();
  };
}