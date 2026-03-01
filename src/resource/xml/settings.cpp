#include "settings.hpp"
#include "util.hpp"

#include <tinyxml2/tinyxml2.h>

#include "../../log.hpp"

#include <format>

#ifdef __EMSCRIPTEN__
  #include "../../util/web_filesystem.hpp"
#endif

using namespace tinyxml2;
using namespace game::util;

namespace game::resource::xml
{
  Settings::Settings(const std::filesystem::path& path)
  {
    XMLDocument document;
    auto pathString = path.string();

    if (document.LoadFile(pathString.c_str()) != XML_SUCCESS)
    {
      logger.error(
          std::format("Could not initialize character save file: {} ({})", pathString, document.ErrorStr()));
      return;
    }

    if (auto root = document.RootElement())
    {
      std::string measurementSystemString{};
      query_string_attribute(root, "MeasurementSystem", &measurementSystemString);
      measurementSystem = measurementSystemString == "Imperial" ? measurement::IMPERIAL : measurement::METRIC;
      root->QueryIntAttribute("Volume", &volume);
      root->QueryFloatAttribute("ColorR", &color.r);
      root->QueryFloatAttribute("ColorG", &color.g);
      root->QueryFloatAttribute("ColorB", &color.b);
      root->QueryFloatAttribute("WindowX", &windowPosition.x);
      root->QueryFloatAttribute("WindowY", &windowPosition.y);
      root->QueryIntAttribute("WindowW", &windowSize.x);
      root->QueryIntAttribute("WindowH", &windowSize.y);
    }

    logger.info(std::format("Initialized settings: {}", pathString));

    isValid = true;
  }

  bool Settings::is_valid() const { return isValid; }

  void Settings::serialize(const std::filesystem::path& path)
  {
    XMLDocument document;
    auto pathString = path.string();

    auto element = document.NewElement("Settings");

    element->SetAttribute("MeasurementSystem", measurementSystem == measurement::IMPERIAL ? "Imperial" : "Metric");
    element->SetAttribute("Volume", volume);
    element->SetAttribute("ColorR", color.r);
    element->SetAttribute("ColorG", color.g);
    element->SetAttribute("ColorB", color.b);
    element->SetAttribute("WindowX", windowPosition.x);
    element->SetAttribute("WindowY", windowPosition.y);
    element->SetAttribute("WindowW", windowSize.x);
    element->SetAttribute("WindowH", windowSize.y);

    document.InsertFirstChild(element);

    if (document.SaveFile(pathString.c_str()) != XML_SUCCESS)
    {
      logger.info(std::format("Failed to initialize settings: {} ({})", pathString, document.ErrorStr()));
      return;
    }

    logger.info(std::format("Saved settings: {}", pathString));

#ifdef __EMSCRIPTEN__
    web_filesystem::flush_async();
#endif
  }
}
