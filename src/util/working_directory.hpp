#pragma once

#include <filesystem>

namespace game::util
{
  class WorkingDirectory
  {
  public:
    enum Type
    {
      DIRECTORY,
      FILE
    };

    std::filesystem::path previous{};
    bool isValid{};

    WorkingDirectory() = default;
    WorkingDirectory(const std::filesystem::path&, Type type = DIRECTORY);
    ~WorkingDirectory();
    bool is_valid() const;
  };
}