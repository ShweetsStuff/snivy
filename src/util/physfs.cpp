#include "physfs.hpp"

#include <physfs.h>

#include <string_view>
#include <vector>

#include "../log.hpp"

namespace game::util::physfs
{
  std::string path_normalize(std::string_view path)
  {
    const bool absolute = !path.empty() && path.front() == '/';
    std::vector<std::string> parts{};
    std::string segment{};

    auto flush_segment = [&]()
    {
      if (segment.empty() || segment == ".")
      {
        segment.clear();
        return;
      }

      if (segment == "..")
      {
        if (!parts.empty() && parts.back() != "..")
          parts.pop_back();
        else if (!absolute)
          parts.emplace_back("..");
      }
      else
      {
        parts.emplace_back(std::move(segment));
      }

      segment.clear();
    };

    for (char ch : path)
    {
      if (ch == '/')
      {
        flush_segment();
        continue;
      }

      segment.push_back(ch);
    }

    flush_segment();

    std::string pathNormalized{};
    if (absolute) pathNormalized.push_back('/');
    for (size_t i = 0; i < parts.size(); ++i)
    {
      if (i > 0) pathNormalized.push_back('/');
      pathNormalized.append(parts[i]);
    }

    if (pathNormalized.empty() && absolute) return "/";
    return pathNormalized;
  }

  std::string error_get() { return PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()); }

  Archive::Archive(const std::filesystem::path& path, const std::string& mount)
  {
    auto pathString = path.string();
    if (!PHYSFS_mount(pathString.c_str(), mount.c_str(), 0))
    {
      logger.error(std::format("Failed to mount archive: {} ({})", pathString, error_get()));
      return;
    }

    *this = std::string(mount);
  }

  Archive::~Archive() { PHYSFS_unmount(c_str()); }

  bool Archive::is_valid() { return !empty(); }

  std::vector<uint8_t> Path::read() const
  {
    auto pathNormalized = path_normalize(*this);
    auto file = PHYSFS_openRead(pathNormalized.c_str());

    if (!file)
    {
      logger.error(
          std::format("Failed to read PhysicsFS file (PHYFSFS_openRead): {} ({})", pathNormalized, error_get()));
      return {};
    }

    auto size = PHYSFS_fileLength(file);
    if (size < 0)
    {
      logger.error(
          std::format("Failed to get PhysicsFS file length (PHYSFS_fileLength): {} ({})", pathNormalized, error_get()));
      return {};
    }

    std::vector<uint8_t> buffer((size_t)size);

    auto read = PHYSFS_readBytes(file, buffer.data(), size);

    PHYSFS_close(file);

    if (read != size)
    {
      logger.error(std::format("Invalid PhysicsFS file read size: {} ({})", pathNormalized, error_get()));
      return {};
    }

    return buffer;
  }

  physfs::Path Path::directory_get() const
  {
    auto pos = find_last_of('/');
    if (pos == std::string_view::npos) return physfs::Path("");
    if (pos == 0) return physfs::Path("/");
    return physfs::Path(substr(0, pos));
  }

  bool Path::is_valid() const
  {
    auto pathNormalized = path_normalize(*this);
    return PHYSFS_exists(pathNormalized.c_str());
  }
}
