#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include <tinyxml2.h>

#include "animation_entry.hpp"
#include "sound_entry.hpp"

#include "../../util/physfs.hpp"
#include "../font.hpp"
#include "anm2.hpp"

namespace game::resource::xml
{
  tinyxml2::XMLError query_string_attribute(tinyxml2::XMLElement*, const char*, std::string*);
  tinyxml2::XMLError query_bool_attribute(tinyxml2::XMLElement*, const char*, bool*);
  tinyxml2::XMLError query_path_attribute(tinyxml2::XMLElement*, const char*, std::filesystem::path*);
  tinyxml2::XMLError query_color_attribute(tinyxml2::XMLElement*, const char*, float*);
  tinyxml2::XMLError query_float_optional_attribute(tinyxml2::XMLElement* element, const char* attribute,
                                                    std::optional<float>& value);
  tinyxml2::XMLError query_int_optional_attribute(tinyxml2::XMLElement* element, const char* attribute,
                                                  std::optional<int>& value);

  void query_event_id(tinyxml2::XMLElement* element, const char* name, const Anm2& anm2, int& eventID);
  void query_layer_id(tinyxml2::XMLElement* element, const char* name, const Anm2& anm2, int& layerID);
  void query_null_id(tinyxml2::XMLElement* element, const char* name, const Anm2& anm2, int& nullID);

  void query_anm2(tinyxml2::XMLElement* element, const char* name, const std::string& archive,
                  const std::string& rootPath, Anm2& anm2, Anm2::Flags flags = {});
  void query_texture(tinyxml2::XMLElement* element, const char* name, const std::string& archive,
                     const std::string& rootPath, Texture& texture);
  void query_sound(tinyxml2::XMLElement* element, const char* name, const std::string& archive,
                   const std::string& rootPath, Audio& sound);
  void query_font(tinyxml2::XMLElement* element, const char* name, const std::string& archive,
                  const std::string& rootPath, Font& font);

  void query_animation_entry(tinyxml2::XMLElement* element, AnimationEntry& animationEntry);
  void query_animation_entry_collection(tinyxml2::XMLElement* element, const char* name,
                                        AnimationEntryCollection& animationEntryCollection);

  void query_sound_entry(tinyxml2::XMLElement* element, const std::string& archive, const std::string& rootPath,
                         SoundEntry& soundEntry, const std::string& attributeName = "Sound");
  void query_sound_entry_collection(tinyxml2::XMLElement* element, const char* name, const std::string& archive,
                                    const std::string& rootPath, SoundEntryCollection& soundEntryCollection,
                                    const std::string& attributeName = "Sound");

  tinyxml2::XMLError document_load(const util::physfs::Path&, tinyxml2::XMLDocument&);
}
