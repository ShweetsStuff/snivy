#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

namespace game
{
#define LEVELS                                                                                                         \
  X(LEVEL_INFO, "[INFO]")                                                                                              \
  X(LEVEL_WARNING, "[WARNING]")                                                                                        \
  X(LEVEL_ERROR, "[ERROR]")                                                                                            \
  X(LEVEL_FATAL, "[FATAL]")

  enum Level
  {
#define X(symbol, string) symbol,
    LEVELS
#undef X
  };

  constexpr std::string_view LEVEL_STRINGS[] = {
#define X(symbol, string) string,
      LEVELS
#undef X
  };
#undef LEVELS

  class Logger
  {
    std::ofstream file{};

  public:
    static std::filesystem::path path();
    void write_raw(const std::string&);
    void write(const Level, const std::string&);
    void info(const std::string&);
    void warning(const std::string&);
    void error(const std::string&);
    void fatal(const std::string&);
    void open(const std::filesystem::path&);
    Logger();
  };

}

extern game::Logger logger;