#pragma once

#include "glm/ext/vector_float4.hpp"

#include <tinyxml2.h>

#include <string>

#include <map>

namespace game::resource
{
  class Dialogue
  {
  public:
    static constexpr auto FULL_LABEL = "Full";
    static constexpr auto POST_DIGEST_LABEL = "PostDigest";
    static constexpr auto BURP_SMALL_LABEL = "BurpSmall";
    static constexpr auto BURP_BIG_LABEL = "BurpBig";
    static constexpr auto FEED_HUNGRY_LABEL = "FeedHungry";
    static constexpr auto FEED_FULL_LABEL = "FeedFull";
    static constexpr auto EAT_HUNGRY_LABEL = "EatHungry";
    static constexpr auto EAT_FULL_LABEL = "EatFull";
    static constexpr auto FOOD_STOLEN_LABEL = "FoodStolen";
    static constexpr auto FOOD_EASED_LABEL = "FoodEased";
    static constexpr auto CAPACITY_LOW_LABEL = "CapacityLow";
    static constexpr auto PERFECT_LABEL = "Perfect";
    static constexpr auto MISS_LABEL = "Miss";
    static constexpr auto RANDOM_LABEL = "StartRandom";
    ;

    class Color
    {
    public:
      int start{};
      int end{};
      glm::vec4 value{};

      Color(tinyxml2::XMLElement*);
    };

    class Animation
    {
    public:
      int at{-1};
      std::string name{};

      Animation(tinyxml2::XMLElement*);
    };

    class Branch
    {
    public:
      std::string content{};
      int nextID{-1};

      Branch(tinyxml2::XMLElement*, std::map<std::string, int>&);
    };

    class Entry
    {
    public:
#define FLAGS                                                                                                          \
  X(NONE, "None")                                                                                                      \
  X(ACTIVATE_WINDOWS, "ActivateWindows")                                                                               \
  X(DEACTIVATE_WINDOWS, "DeactivateWindows")                                                                           \
  X(ONLY_INFO, "OnlyInfo")                                                                                             \
  X(ACTIVATE_CHEATS, "ActivateCheats")

      enum Flag
      {
#define X(symbol, name) symbol,
        FLAGS
#undef X
      };

      static constexpr const char* FLAG_STRINGS[] = {
#define X(symbol, name) name,
          FLAGS
#undef X
      };

#undef FLAGS

      std::string content{};
      std::vector<Color> colors{};
      std::vector<Animation> animations{};
      std::vector<Branch> branches{};
      int nextID{-1};
      Flag flag{Flag::NONE};

      Entry(tinyxml2::XMLElement*, std::map<std::string, int>&);
    };

    std::map<std::string, int> labelMap;
    std::map<int, Entry> entryMap{};

    std::vector<int> eatHungryIDs{};
    std::vector<int> eatFullIDs{};
    std::vector<int> feedHungryIDs{};
    std::vector<int> feedFullIDs{};
    std::vector<int> burpSmallIDs{};
    std::vector<int> burpBigIDs{};
    std::vector<int> fullIDs{};
    std::vector<int> foodStolenIDs{};
    std::vector<int> foodEasedIDs{};
    std::vector<int> perfectIDs{};
    std::vector<int> postDigestIDs{};
    std::vector<int> missIDs{};
    std::vector<int> capacityLowIDs{};
    std::vector<int> randomIDs{};

    Dialogue(const std::string&);
    Entry* get(const std::string&);
    Entry* get(int = -1);
    Entry* next(Entry*);
  };
}