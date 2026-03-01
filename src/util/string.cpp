#include "string.hpp"

namespace game::util::string
{
  std::string to_lower(const std::string& string)
  {
    std::string transformed = string;
    std::ranges::transform(transformed, transformed.begin(), [](const unsigned char c) { return std::tolower(c); });
    return transformed;
  }
}