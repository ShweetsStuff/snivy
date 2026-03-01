#pragma once

#include <filesystem>
#include <vector>

namespace game::util::physfs
{
  class Archive : public std::string
  {
  public:
    using std::string::operator=;

    Archive(const std::filesystem::path&, const std::string& mount);
    ~Archive();

    bool is_valid();
  };

  class Path : public std::string
  {
  public:
    bool is_valid() const;

    Path directory_get() const;
    std::vector<uint8_t> read() const;
  };

  std::string error_get();
}
