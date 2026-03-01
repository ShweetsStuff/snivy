#include "font.hpp"

#include "../log.hpp"

using namespace game::util;

static ImFont* default_font_fallback_get()
{
  auto& io = ImGui::GetIO();

  if (io.FontDefault) return io.FontDefault;
  if (!io.Fonts->Fonts.empty()) return io.Fonts->Fonts.front();
  return io.Fonts->AddFontDefault();
}

namespace game::resource
{
  Font::Font(const std::filesystem::path& path, float size)
  {
    auto pathString = path.string();
    std::error_code ec;

    if (!std::filesystem::is_regular_file(path, ec))
    {
      logger.error(std::format("Failed to initialize font: {} (file not found)", pathString));
      internal = default_font_fallback_get();
      if (internal)
        logger.info(std::format("Falling back to default ImGui font: {}", pathString));
      else
        logger.error("Failed to initialize fallback ImGui default font");
      return;
    }

    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;

    internal = ImGui::GetIO().Fonts->AddFontFromFileTTF(pathString.c_str(), size, &config);

    if (internal)
      logger.info(std::format("Initialized font: {}", pathString));
    else
    {
      logger.error(std::format("Failed to initialize font: {}", pathString));
      internal = default_font_fallback_get();
      if (internal)
        logger.info(std::format("Falling back to default ImGui font: {}", pathString));
      else
        logger.error("Failed to initialize fallback ImGui default font");
    }
  }

  Font::Font(const physfs::Path& path, float size)
  {
    if (!path.is_valid())
    {
      logger.error(
          std::format("Failed to initialize font from PhysicsFS path: {} ({})", path.c_str(), physfs::error_get()));
      internal = default_font_fallback_get();
      if (internal)
        logger.info(std::format("Falling back to default ImGui font: {}", path.c_str()));
      else
        logger.error("Failed to initialize fallback ImGui default font");
      return;
    }

    auto buffer = path.read();

    if (buffer.empty())
    {
      logger.error(
          std::format("Failed to initialize font from PhysicsFS path: {} ({})", path.c_str(), physfs::error_get()));
      internal = default_font_fallback_get();
      if (internal)
        logger.info(std::format("Falling back to default ImGui font: {}", path.c_str()));
      else
        logger.error("Failed to initialize fallback ImGui default font");
      return;
    }

    if (buffer.size() <= 100)
    {
      logger.error(std::format("Failed to initialize font from PhysicsFS path: {} (buffer too small)", path.c_str()));
      internal = default_font_fallback_get();
      if (internal)
        logger.info(std::format("Falling back to default ImGui font: {}", path.c_str()));
      else
        logger.error("Failed to initialize fallback ImGui default font");
      return;
    }

    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;

    internal = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer.data(), (int)buffer.size(), size, &config);

    if (internal)
      logger.info(std::format("Initialized font: {}", path.c_str()));
    else
    {
      logger.error(std::format("Failed to initialize font: {}", path.c_str()));
      internal = default_font_fallback_get();
      if (internal)
        logger.info(std::format("Falling back to default ImGui font: {}", path.c_str()));
      else
        logger.error("Failed to initialize fallback ImGui default font");
    }
  }

  ImFont* Font::get() { return internal; };
}
