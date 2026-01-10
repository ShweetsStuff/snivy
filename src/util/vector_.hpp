#pragma once

#include <vector>

namespace game::util::vector
{
  template <typename T> bool in_bounds(std::vector<T>& vector, int index)
  {
    return (index >= 0 && index < vector.size());
  }

  template <typename T> T* find(std::vector<T>& vector, int index)
  {
    if (!in_bounds(vector, index)) return nullptr;
    return &vector[index];
  }
}
