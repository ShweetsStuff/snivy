#include "xml_.h"

using namespace tinyxml2;

namespace game::util::xml
{
  XMLError query_string_attribute(XMLElement* element, const char* attribute, std::string* value)
  {
    const char* temp = nullptr;
    auto result = element->QueryStringAttribute(attribute, &temp);
    if (result == XML_SUCCESS && temp && value) *value = temp;
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
}