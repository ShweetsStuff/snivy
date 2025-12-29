#pragma once

#include <optional>
#include <tinyxml2/tinyxml2.h>

#include <filesystem>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "audio.h"
#include "texture.h"

namespace game::anm2
{
  enum Type
  {
    NONE,
    ROOT,
    LAYER,
    NULL_,
    TRIGGER
  };

  class Info
  {
  public:
    int fps = 30;

    Info() = default;
    Info(tinyxml2::XMLElement*);
  };

  class Spritesheet
  {
  public:
    std::filesystem::path path{};
    resource::Texture texture{};

    Spritesheet(tinyxml2::XMLElement*, int&);
  };

  class Layer
  {
  public:
    std::string name{"New Layer"};
    int spritesheetID{-1};
    Layer(tinyxml2::XMLElement*, int&);
  };

  class Null
  {
  public:
    std::string name{"New Null"};
    bool isShowRect{};
    Null(tinyxml2::XMLElement*, int&);
  };

  class Event
  {
  public:
    std::string name{"New Event"};
    Event(tinyxml2::XMLElement*, int&);
  };

  class Sound
  {
  public:
    std::filesystem::path path{};
    resource::Audio audio{};

    Sound(tinyxml2::XMLElement*, int&);
  };

  class Content
  {
  public:
    std::map<int, Spritesheet> spritesheets{};
    std::map<int, Layer> layers{};
    std::map<int, Null> nulls{};
    std::map<int, Event> events{};
    std::map<int, Sound> sounds{};

    Content() = default;
    Content(tinyxml2::XMLElement*);
  };

  struct Frame
  {
    glm::vec2 crop{};
    glm::vec2 position{};
    glm::vec2 pivot{};
    glm::vec2 size{};
    glm::vec2 scale{100, 100};
    float rotation{};
    int duration{};
    glm::vec4 tint{1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec3 colorOffset{};
    bool isInterpolated{};
    int eventID{-1};
    int soundID{-1};
    int atFrame{-1};

    bool isVisible{true};

    Frame() = default;
    Frame(tinyxml2::XMLElement*, Type);
  };

  struct FrameOptional
  {
    std::optional<glm::vec2> crop{};
    std::optional<glm::vec2> position{};
    std::optional<glm::vec2> pivot{};
    std::optional<glm::vec2> size{};
    std::optional<glm::vec2> scale{};
    std::optional<float> rotation{};
    std::optional<glm::vec4> tint{};
    std::optional<glm::vec3> colorOffset{};
    std::optional<bool> isInterpolated{};
    std::optional<bool> isVisible{};
  };

  class Item
  {
  public:
    std::vector<Frame> frames{};
    bool isVisible{};

    Item() = default;
    Item(tinyxml2::XMLElement*, Type, int&);
  };

  class Animation
  {
  public:
    std::string name{"New Animation"};
    int frameNum{};
    bool isLoop{};

    Item rootAnimation{};
    std::unordered_map<int, Item> layerAnimations{};
    std::vector<int> layerOrder{};
    std::map<int, Item> nullAnimations{};
    Item triggers{};

    Animation() = default;
    Animation(tinyxml2::XMLElement*);
  };

  class Animations
  {
  public:
    std::string defaultAnimation{};
    std::vector<Animation> items{};
    std::unordered_map<std::string, int> map{};
    std::unordered_map<int, std::string> mapReverse{};

    Animations() = default;
    Animations(tinyxml2::XMLElement*);
  };

  class Anm2
  {
  public:
    Info info;
    Content content{};
    Animations animations{};

    Anm2() = default;
    Anm2(const std::filesystem::path&);
  };
}
