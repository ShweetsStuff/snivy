#include "font.hpp"

#include "../log.hpp"

using namespace game::util;

namespace game::resource
{
  Font::Font(const std::filesystem::path& path, float size)
  {
    auto pathString = path.string();

    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;

    internal = ImGui::GetIO().Fonts->AddFontFromFileTTF(pathString.c_str(), size, &config);

    if (internal)
      logger.info(std::format("Initialized font: {}", pathString));
    else
      logger.error(std::format("Failed to initialize font: {}", pathString));
  }

  Font::Font(const physfs::Path& path, float size)
  {
    if (!path.is_valid())
    {
      logger.error(
          std::format("Failed to initialize font from PhysicsFS path: {} ({})", path.c_str(), physfs::error_get()));
      return;
    }

    auto buffer = path.read();

    if (buffer.empty())
    {
      logger.error(
          std::format("Failed to initialize font from PhysicsFS path: {} ({})", path.c_str(), physfs::error_get()));
      return;
    }

    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;

    internal = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer.data(), (int)buffer.size(), size, &config);

    if (internal)
      logger.info(std::format("Initialized font: {}", path.c_str()));
    else
      logger.error(std::format("Failed to initialize font: {}", path.c_str()));
  }

  ImFont* Font::get() { return internal; };
}
