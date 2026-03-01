#include "working_directory.hpp"

#include <format>
#include <iostream>

namespace game::util
{
  WorkingDirectory::WorkingDirectory(const std::filesystem::path& path, Type type)
  {
    std::error_code ec{};
    previous = std::filesystem::current_path(ec);
    if (ec)
    {
      std::cout << std::format("Could not query current directory: {}", ec.message()) << "\n";
      previous.clear();
      ec.clear();
    }

    if (type == FILE && path.has_parent_path())
    {
      std::filesystem::path parentPath = path.parent_path();
      std::filesystem::current_path(parentPath, ec);
      if (ec)
        std::cout << std::format("Could not set current directory to {}: {}", parentPath.string(), ec.message())
                  << "\n";
    }
    else if (std::filesystem::is_directory(path))
    {
      std::filesystem::current_path(path, ec);
      if (ec)
        std::cout << std::format("Could not set current directory to {}: {}", path.string(), ec.message()) << "\n";
    }

    isValid = true;
  }

  WorkingDirectory::~WorkingDirectory()
  {
    if (previous.empty() || !isValid) return;

    std::error_code ec{};
    std::filesystem::current_path(previous, ec);
    if (ec)
      std::cout << std::format("Could not restore current directory to {}: {}", previous.string(), ec.message())
                << "\n";
  }

  bool WorkingDirectory::is_valid() const { return isValid; }
}