#include "animation_entry.hpp"

#include "../../util/vector.hpp"

namespace game::resource::xml
{
  const std::string& AnimationEntryCollection::get()
  {
    return at(util::vector::random_index_weighted(*this, [](const auto& entry) { return entry.weight; })).animation;
  }
}
