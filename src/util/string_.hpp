#pragma once

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>

namespace game::util::string
{
  template <typename Number>
  std::string format_commas(Number value, int decimalDigits = -1)
  {
    static_assert(std::is_arithmetic_v<Number>, "format_commas requires numeric types");

    std::ostringstream stream;
    if (decimalDigits >= 0)
    {
      stream.setf(std::ios::fixed);
      stream << std::setprecision(decimalDigits);
    }
    stream << value;
    std::string text = stream.str();

    std::string exponent;
    if (auto exponentPos = text.find_first_of("eE"); exponentPos != std::string::npos)
    {
      exponent = text.substr(exponentPos);
      text = text.substr(0, exponentPos);
    }

    std::string fraction;
    if (auto decimalPos = text.find('.'); decimalPos != std::string::npos)
    {
      fraction = text.substr(decimalPos);
      text = text.substr(0, decimalPos);
    }

    bool isNegative = false;
    if (!text.empty() && text.front() == '-')
    {
      isNegative = true;
      text.erase(text.begin());
    }

    std::string formattedInteger;
    formattedInteger.reserve(text.size() + (text.size() / 3));

    int digitCount = 0;
    for (auto it = text.rbegin(); it != text.rend(); ++it)
    {
      if (digitCount != 0 && digitCount % 3 == 0) formattedInteger.push_back(',');
      formattedInteger.push_back(*it);
      ++digitCount;
    }
    std::reverse(formattedInteger.begin(), formattedInteger.end());

    if (isNegative) formattedInteger.insert(formattedInteger.begin(), '-');

    return formattedInteger + fraction + exponent;
  }
}
