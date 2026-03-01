#include "util.hpp"

#include <format>

#include "../../util/physfs.hpp"
#include "../../util/string.hpp"

#include "../../log.hpp"

using namespace tinyxml2;
using namespace game::util;

namespace game::resource::xml
{
  XMLError query_string_attribute(XMLElement* element, const char* attribute, std::string* value)
  {
    const char* temp = nullptr;
    auto result = element->QueryStringAttribute(attribute, &temp);
    if (result == XML_SUCCESS && temp && value) *value = temp;
    return result;
  }

  XMLError query_bool_attribute(XMLElement* element, const char* attribute, bool* value)
  {
    std::string temp{};
    auto result = query_string_attribute(element, attribute, &temp);
    temp = string::to_lower(temp);
    if (value) *value = temp == "true" || temp == "1" ? true : false;
    return result;
  }

  XMLError query_path_attribute(XMLElement* element, const char* attribute, std::filesystem::path* value)
  {
    std::string temp{};
    auto result = query_string_attribute(element, attribute, &temp);
    if (value) *value = std::filesystem::path(temp);
    return result;
  }

  XMLError query_color_attribute(XMLElement* element, const char* attribute, float* value)
  {
    int temp{};
    auto result = element->QueryIntAttribute(attribute, &temp);
    if (result == XML_SUCCESS && value) *value = (temp / 255.0f);
    return result;
  }

  XMLError query_float_optional_attribute(XMLElement* element, const char* attribute, std::optional<float>& value)
  {
    value.emplace();
    auto result = element->QueryFloatAttribute(attribute, &*value);
    if (result == XML_NO_ATTRIBUTE) value.reset();
    return result;
  }

  XMLError query_int_optional_attribute(XMLElement* element, const char* attribute, std::optional<int>& value)
  {
    value.emplace();
    auto result = element->QueryIntAttribute(attribute, &*value);
    if (result == XML_NO_ATTRIBUTE) value.reset();
    return result;
  }

  XMLError document_load(const physfs::Path& path, XMLDocument& document)
  {
    if (!path.is_valid())
    {
      logger.error(std::format("Failed to open XML document: {} ({})", path.c_str(), physfs::error_get()));
      return XML_ERROR_FILE_NOT_FOUND;
    }

    auto buffer = path.read();

    if (buffer.empty())
    {
      logger.error(std::format("Failed to read XML document: {} ({})", path.c_str(), physfs::error_get()));
      return XML_ERROR_FILE_COULD_NOT_BE_OPENED;
    }

    auto result = document.Parse((const char*)buffer.data(), buffer.size());
    if (result != XML_SUCCESS)
      logger.error(std::format("Failed to parse XML document: {} ({})", path.c_str(), document.ErrorStr()));

    return result;
  }

  void query_event_id(XMLElement* element, const char* name, const Anm2& anm2, int& eventID)
  {
    std::string string{};
    query_string_attribute(element, name, &string);
    if (anm2.eventMap.contains(string))
      eventID = anm2.eventMap.at(string);
    else
    {
      logger.error(std::format("Could not query anm2 event ID: {} ({})", string, anm2.path));
      eventID = -1;
    }
  }

  void query_layer_id(XMLElement* element, const char* name, const Anm2& anm2, int& layerID)
  {
    std::string string{};
    query_string_attribute(element, name, &string);
    if (anm2.layerMap.contains(string))
      layerID = anm2.layerMap.at(string);
    else
    {
      logger.error(std::format("Could not query anm2 layer ID: {} ({})", string, anm2.path));
      layerID = -1;
    }
  }

  void query_null_id(XMLElement* element, const char* name, const Anm2& anm2, int& nullID)
  {
    std::string string{};
    query_string_attribute(element, name, &string);
    if (anm2.nullMap.contains(string))
      nullID = anm2.nullMap.at(string);
    else
    {
      logger.error(std::format("Could not query anm2 null ID: {} ({})", string, anm2.path));
      nullID = -1;
    }
  }

  void query_anm2(XMLElement* element, const char* name, const std::string& archive, const std::string& rootPath,
                  Anm2& anm2, Anm2::Flags flags)
  {
    std::string string{};
    query_string_attribute(element, name, &string);
    anm2 = Anm2(physfs::Path(archive + "/" + rootPath + "/" + string), flags);
  }

  void query_texture(XMLElement* element, const char* name, const std::string& archive, const std::string& rootPath,
                     Texture& texture)
  {
    std::string string{};
    query_string_attribute(element, name, &string);
    texture = Texture(physfs::Path(archive + "/" + rootPath + "/" + string));
  }

  void query_sound(XMLElement* element, const char* name, const std::string& archive, const std::string& rootPath,
                   Audio& sound)
  {
    std::string string{};
    query_string_attribute(element, name, &string);
    sound = Audio(physfs::Path(archive + "/" + rootPath + "/" + string));
  }

  void query_font(XMLElement* element, const char* name, const std::string& archive, const std::string& rootPath,
                  Font& font)
  {
    std::string string{};
    query_string_attribute(element, name, &string);
    font = Font(physfs::Path(archive + "/" + rootPath + "/" + string));
  }

  void query_animation_entry(XMLElement* element, AnimationEntry& animationEntry)
  {
    query_string_attribute(element, "Animation", &animationEntry.animation);
    element->QueryFloatAttribute("Weight", &animationEntry.weight);
  }

  void query_animation_entry_collection(XMLElement* element, const char* name,
                                        AnimationEntryCollection& animationEntryCollection)
  {
    for (auto child = element->FirstChildElement(name); child; child = child->NextSiblingElement(name))
      query_animation_entry(child, animationEntryCollection.emplace_back());
  }

  void query_sound_entry(XMLElement* element, const std::string& archive, const std::string& rootPath,
                         SoundEntry& soundEntry, const std::string& attributeName)
  {
    query_sound(element, attributeName.c_str(), archive, rootPath, soundEntry.sound);
    element->QueryFloatAttribute("Weight", &soundEntry.weight);
  }

  void query_sound_entry_collection(XMLElement* element, const char* name, const std::string& archive,
                                    const std::string& rootPath, SoundEntryCollection& soundEntryCollection,
                                    const std::string& attributeName)
  {
    for (auto child = element->FirstChildElement(name); child; child = child->NextSiblingElement(name))
      query_sound_entry(child, archive, rootPath, soundEntryCollection.emplace_back(), attributeName);
  }
}
