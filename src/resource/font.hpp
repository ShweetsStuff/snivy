#pragma once

#include <filesystem>

#include <imgui.h>

#include "../util/physfs.hpp"

namespace game::resource
{
  class Font
  {
    ImFont* internal{};

  public:
    static constexpr auto NORMAL = 20;
    static constexpr auto ABOVE_AVERAGE = 24;
    static constexpr auto BIG = 30;
    static constexpr auto HEADER_3 = 40;
    static constexpr auto HEADER_2 = 50;
    static constexpr auto HEADER_1 = 60;

    Font() = default;
    Font(const std::filesystem::path&, float = NORMAL);
    Font(const util::physfs::Path&, float = NORMAL);
    ImFont* get();
  };
}
