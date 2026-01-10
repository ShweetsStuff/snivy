#pragma once

#include <tinyxml2.h>

#include <filesystem>
#include <string>

namespace game::util::xml
{
  tinyxml2::XMLError query_string_attribute(tinyxml2::XMLElement*, const char*, std::string*);
  tinyxml2::XMLError query_path_attribute(tinyxml2::XMLElement*, const char*, std::filesystem::path*);
  tinyxml2::XMLError query_color_attribute(tinyxml2::XMLElement*, const char*, float*);
}