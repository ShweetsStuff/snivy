#include "time.hpp"

#include <chrono>
#include <iomanip>

namespace game::util::time
{
  std::string get(const char* format)
  {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &nowTime);
#else
    localtime_r(&nowTime, &localTime);
#endif
    std::ostringstream timeString;
    timeString << std::put_time(&localTime, format);
    return timeString.str();
  }
}
