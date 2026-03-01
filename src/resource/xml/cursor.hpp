#pragma once

#include "util.hpp"

namespace game::resource::xml
{
  class Cursor
  {
  public:
    struct Animations
    {
      AnimationEntryCollection idle{};
      AnimationEntryCollection hover{};
      AnimationEntryCollection grab{};
      AnimationEntryCollection pan{};
      AnimationEntryCollection zoom{};
      AnimationEntryCollection return_{};
    };

    struct Sounds
    {
      SoundEntryCollection grab{};
      SoundEntryCollection release{};
      SoundEntryCollection throw_{};
    };

    Animations animations{};
    Sounds sounds{};
    Anm2 anm2{};

    bool isValid{};

    Cursor() = default;
    Cursor(const util::physfs::Path&);

    bool is_valid() const;
  };
}
