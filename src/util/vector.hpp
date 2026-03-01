#pragma once

#include <utility>
#include <vector>

#include "math.hpp"

namespace game::util::vector
{
  template <typename T> bool in_bounds(std::vector<T>& vector, int index)
  {
    return (index >= 0 && index < (int)vector.size());
  }

  template <typename T> T* find(std::vector<T>& vector, int index)
  {
    if (!in_bounds(vector, index)) return nullptr;
    return &vector[index];
  }

  template <typename T> T* find(std::vector<T>& vector, T& value)
  {
    auto it = find(vector.begin(), vector.end(), value);
    return (it == vector.end()) ? nullptr : std::addressof(*it);
  }

  template <typename T> int index_get(std::vector<T>& vector, T& value)
  {
    auto it = find(vector.begin(), vector.end(), value);
    return (it == vector.end()) ? -1 : (int)std::distance(vector.begin(), it);
  }

  template <typename T, typename... Args> int emplace_index(std::vector<T>& vector, Args&&... args)
  {
    auto index = (int)vector.size();
    vector.emplace_back(std::forward<Args>(args)...);
    return index;
  }

  template <typename T> int push_index(std::vector<T>& vector, const T& value)
  {
    auto index = (int)vector.size();
    vector.push_back(value);
    return index;
  }

  template <typename T> int push_index(std::vector<T>& vector, T&& value)
  {
    auto index = (int)vector.size();
    vector.push_back(std::move(value));
    return index;
  }

  template <typename T, typename WeightFunction>
  int random_index_weighted(const std::vector<T>& vector, WeightFunction weightFunction)
  {
    if (vector.empty()) return -1;
    if (vector.size() == 1) return 0;

    float total{};
    float accumulator{};

    for (auto& item : vector)
    {
      auto weight = (float)(weightFunction(item));
      if (weight > 0.0f) total += weight;
    }

    if (total <= 0.0f) return -1;

    float randomValue = math::random_max(total);
    int lastIndex = -1;

    for (int i = 0; i < (int)vector.size(); i++)
    {
      auto weight = (float)(weightFunction(vector[i]));
      if (weight <= 0.0f) continue;

      lastIndex = i;
      accumulator += weight;
      if (randomValue < accumulator) return i;
    }

    return lastIndex;
  }
}
