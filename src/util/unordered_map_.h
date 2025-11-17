#pragma once

#include <unordered_map>

namespace game::util::unordered_map
{
  template <typename T0, typename T1> T1* find(std::unordered_map<T0, T1>& map, T0 key)
  {
    auto it = map.find(key);
    if (it != map.end()) return &it->second;
    return nullptr;
  }
}