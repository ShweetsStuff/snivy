#pragma once

#include <vector>

#include "../texture.hpp"
#include "../../util/physfs.hpp"

namespace game::resource::xml
{
  class Area
  {
  public:
    struct Entry
    {
      Texture texture{};
      float gravity{0.95f};
      float friction{0.80f};
      float airResistance{0.975f};
    };

    std::vector<Entry> areas{};
    bool isValid{};

    Area() = default;
    Area(const util::physfs::Path&);

    bool is_valid() const;
  };
}
