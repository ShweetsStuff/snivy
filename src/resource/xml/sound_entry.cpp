#include "sound_entry.hpp"

#include "../../util/vector.hpp"

namespace game::resource::xml
{
  Audio& SoundEntryCollection::get()
  {
    return at(util::vector::random_index_weighted(*this, [](const auto& entry) { return entry.weight; })).sound;
  }

  void SoundEntryCollection::play()
  {
    at(util::vector::random_index_weighted(*this, [](const auto& entry) { return entry.weight; })).play();
  }
}
