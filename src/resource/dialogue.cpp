#include "dialogue.h"

#include <iostream>

#include "../util/map_.h"
#include "../util/xml_.h"

using namespace tinyxml2;
using namespace game::util;

namespace game::resource
{
  void label_map_query(XMLElement* element, std::map<std::string, int>& labelMap, const char* attribute, int& id)
  {
    std::string label{};
    xml::query_string_attribute(element, attribute, &label);
    if (auto foundID = map::find(labelMap, label))
      id = *foundID;
    else
      id = -1;
  }

  Dialogue::Color::Color(XMLElement* element)
  {
    if (!element) return;
    element->QueryIntAttribute("Start", &start);
    element->QueryIntAttribute("End", &end);
    xml::query_color_attribute(element, "R", &value.r);
    xml::query_color_attribute(element, "G", &value.g);
    xml::query_color_attribute(element, "B", &value.b);
    xml::query_color_attribute(element, "A", &value.a);
  }

  Dialogue::Animation::Animation(XMLElement* element)
  {
    if (!element) return;
    element->QueryIntAttribute("At", &at);
    xml::query_string_attribute(element, "Name", &name);
  }

  Dialogue::Branch::Branch(XMLElement* element, std::map<std::string, int>& labelMap)
  {
    if (!element) return;
    label_map_query(element, labelMap, "Label", nextID);
    xml::query_string_attribute(element, "Content", &content);
  }

  Dialogue::Entry::Entry(XMLElement* element, std::map<std::string, int>& labelMap)
  {
    if (!element) return;

    xml::query_string_attribute(element, "Content", &content);
    label_map_query(element, labelMap, "Next", nextID);

    std::string flagString{};
    xml::query_string_attribute(element, "Flag", &flagString);

    for (int i = 0; i < std::size(FLAG_STRINGS); i++)
    {
      if (flagString == FLAG_STRINGS[i])
      {
        flag = (Flag)i;
        break;
      }
    }

    for (auto child = element->FirstChildElement("Color"); child; child = child->NextSiblingElement("Color"))
      colors.emplace_back(child);

    for (auto child = element->FirstChildElement("Animation"); child; child = child->NextSiblingElement("Animation"))
      animations.emplace_back(child);

    for (auto child = element->FirstChildElement("Branch"); child; child = child->NextSiblingElement("Branch"))
      branches.emplace_back(child, labelMap);
  }

  Dialogue::Dialogue(const std::string& path)
  {
    XMLDocument document;

    if (document.LoadFile(path.c_str()) != XML_SUCCESS)
    {
      std::cout << "Failed to initialize dialogue: " << document.ErrorStr() << "\n";
      return;
    }

    auto element = document.RootElement();
    int id{};

    for (auto child = element->FirstChildElement("Entry"); child; child = child->NextSiblingElement("Entry"))
    {
      std::string label{};
      xml::query_string_attribute(child, "Label", &label);
      labelMap.emplace(label, id++);
    }

    id = 0;

    for (auto child = element->FirstChildElement("Entry"); child; child = child->NextSiblingElement("Entry"))
      entryMap.emplace(id++, Entry(child, labelMap));

    for (auto& [label, id] : labelMap)
    {
      if (label.starts_with(BURP_SMALL_LABEL)) burpSmallIDs.emplace_back(id);
      if (label.starts_with(BURP_BIG_LABEL)) burpBigIDs.emplace_back(id);
      if (label.starts_with(EAT_HUNGRY_LABEL)) eatHungryIDs.emplace_back(id);
      if (label.starts_with(EAT_FULL_LABEL)) eatFullIDs.emplace_back(id);
      if (label.starts_with(FULL_LABEL)) fullIDs.emplace_back(id);
      if (label.starts_with(CAPACITY_LOW_LABEL)) capacityLowIDs.emplace_back(id);
      if (label.starts_with(FEED_HUNGRY_LABEL)) feedHungryIDs.emplace_back(id);
      if (label.starts_with(FEED_FULL_LABEL)) feedFullIDs.emplace_back(id);
      if (label.starts_with(FOOD_STOLEN_LABEL)) foodStolenIDs.emplace_back(id);
      if (label.starts_with(FOOD_EASED_LABEL)) foodEasedIDs.emplace_back(id);
      if (label.starts_with(PERFECT_LABEL)) perfectIDs.emplace_back(id);
      if (label.starts_with(MISS_LABEL)) missIDs.emplace_back(id);
      if (label.starts_with(POST_DIGEST_LABEL)) postDigestIDs.emplace_back(id);
      if (label.starts_with(RANDOM_LABEL)) randomIDs.emplace_back(id);
    }

    std::cout << "Initialzed dialogue: " << path << "\n";
  }

  Dialogue::Entry* Dialogue::get(int id)
  {
    if (id == -1) return nullptr;
    return map::find(entryMap, id);
  }

  Dialogue::Entry* Dialogue::get(const std::string& label)
  {
    auto id = map::find(labelMap, label);
    if (!id) return nullptr;
    return get(*id);
  }

  Dialogue::Entry* Dialogue::next(Dialogue::Entry* entry) { return get(entry->nextID); }
}