#include "anm2.hpp"
#include "util.hpp"

#include "../../util/working_directory.hpp"

#include "../../log.hpp"

#include <ranges>

using namespace tinyxml2;
using namespace game::util;

namespace game::resource::xml
{
  Anm2::Anm2(const Anm2&) = default;
  Anm2::Anm2(Anm2&&) = default;
  Anm2& Anm2::operator=(const Anm2&) = default;
  Anm2& Anm2::operator=(Anm2&&) = default;

  void Anm2::init(XMLDocument& document, Flags flags, const physfs::Path& archive)
  {
    this->flags = flags;

    auto element = document.RootElement();
    if (!element) return;

    auto parse_frame = [&](XMLElement* element, Type type, int spritesheetID = -1)
    {
      Frame frame{};
      if (!element) return frame;

      if (type != TRIGGER)
      {
        element->QueryFloatAttribute("XPosition", &frame.position.x);
        element->QueryFloatAttribute("YPosition", &frame.position.y);
        if (type == LAYER)
        {
          if (element->FindAttribute("RegionId") && spritesheets.contains(spritesheetID))
          {
            auto& spritesheet = spritesheets.at(spritesheetID);
            element->QueryIntAttribute("RegionId", &frame.regionID);
            auto& region = spritesheet.regions.at(frame.regionID);
            frame.crop = region.crop;
            frame.size = region.size;
            frame.pivot = region.origin == Spritesheet::Region::Origin::CENTER
                              ? glm::vec2(glm::ivec2(frame.size * 0.5f))
                          : region.origin == Spritesheet::Region::Origin::TOP_LEFT ? glm::vec2{}
                                                                                   : region.pivot;
          }
          else
          {
            element->QueryFloatAttribute("XPivot", &frame.pivot.x);
            element->QueryFloatAttribute("YPivot", &frame.pivot.y);
            element->QueryFloatAttribute("XCrop", &frame.crop.x);
            element->QueryFloatAttribute("YCrop", &frame.crop.y);
            element->QueryFloatAttribute("Width", &frame.size.x);
            element->QueryFloatAttribute("Height", &frame.size.y);
          }
        }
        element->QueryFloatAttribute("XScale", &frame.scale.x);
        element->QueryFloatAttribute("YScale", &frame.scale.y);
        element->QueryIntAttribute("Delay", &frame.duration);
        element->QueryBoolAttribute("Visible", &frame.isVisible);
        xml::query_color_attribute(element, "RedTint", &frame.tint.r);
        xml::query_color_attribute(element, "GreenTint", &frame.tint.g);
        xml::query_color_attribute(element, "BlueTint", &frame.tint.b);
        xml::query_color_attribute(element, "AlphaTint", &frame.tint.a);
        xml::query_color_attribute(element, "RedOffset", &frame.colorOffset.r);
        xml::query_color_attribute(element, "GreenOffset", &frame.colorOffset.g);
        xml::query_color_attribute(element, "BlueOffset", &frame.colorOffset.b);
        element->QueryFloatAttribute("Rotation", &frame.rotation);
        element->QueryBoolAttribute("Interpolated", &frame.isInterpolated);
      }
      else
      {
        for (auto child = element->FirstChildElement("Sound"); child; child = child->NextSiblingElement("Sound"))
        {
          int soundID{};
          child->QueryIntAttribute("Id", &soundID);
          frame.soundIDs.emplace_back(soundID);
        }

        element->QueryIntAttribute("EventId", &frame.eventID);
        element->QueryIntAttribute("AtFrame", &frame.atFrame);
      }

      return frame;
    };

    auto parse_item = [&](XMLElement* element, Type type, int& id)
    {
      Item item{};
      if (!element) return item;

      if (type == LAYER) element->QueryIntAttribute("LayerId", &id);
      if (type == NULL_) element->QueryIntAttribute("NullId", &id);

      element->QueryBoolAttribute("Visible", &item.isVisible);

      for (auto child = type == TRIGGER ? element->FirstChildElement("Trigger") : element->FirstChildElement("Frame");
           child; child = type == TRIGGER ? child->NextSiblingElement("Trigger") : child->NextSiblingElement("Frame"))
        item.frames.emplace_back(parse_frame(child, type, type == LAYER ? layers.at(id).spritesheetID : -1));

      return item;
    };

    auto parse_animation = [&](XMLElement* element)
    {
      Animation animation{};
      if (!element) return animation;

      xml::query_string_attribute(element, "Name", &animation.name);
      element->QueryIntAttribute("FrameNum", &animation.frameNum);
      element->QueryBoolAttribute("Loop", &animation.isLoop);

      int id{-1};

      if (auto rootAnimationElement = element->FirstChildElement("RootAnimation"))
        animation.rootAnimation = parse_item(rootAnimationElement, ROOT, id);

      if (auto layerAnimationsElement = element->FirstChildElement("LayerAnimations"))
      {
        for (auto child = layerAnimationsElement->FirstChildElement("LayerAnimation"); child;
             child = child->NextSiblingElement("LayerAnimation"))
        {
          auto layerAnimation = parse_item(child, LAYER, id);
          animation.layerOrder.push_back(id);
          animation.layerAnimations.emplace(id, std::move(layerAnimation));
        }
      }

      if (auto nullAnimationsElement = element->FirstChildElement("NullAnimations"))
      {
        for (auto child = nullAnimationsElement->FirstChildElement("NullAnimation"); child;
             child = child->NextSiblingElement("NullAnimation"))
        {
          auto nullAnimation = parse_item(child, NULL_, id);
          animation.nullAnimations.emplace(id, std::move(nullAnimation));
        }
      }

      if (auto triggersElement = element->FirstChildElement("Triggers"))
        animation.triggers = parse_item(triggersElement, TRIGGER, id);

      return animation;
    };

    if (auto infoElement = element->FirstChildElement("Info")) infoElement->QueryIntAttribute("Fps", &fps);

    if (auto contentElement = element->FirstChildElement("Content"))
    {
      if (auto spritesheetsElement = contentElement->FirstChildElement("Spritesheets"))
      {
        for (auto child = spritesheetsElement->FirstChildElement("Spritesheet"); child;
             child = child->NextSiblingElement("Spritesheet"))
        {
          int spritesheetId{};
          Spritesheet spritesheet{};
          child->QueryIntAttribute("Id", &spritesheetId);
          xml::query_string_attribute(child, "Path", &spritesheet.path);

          if ((this->flags & NO_SPRITESHEETS) != 0)
            spritesheet.texture = Texture();
          else if (!archive.empty())
            spritesheet.texture = Texture(physfs::Path(archive + "/" + spritesheet.path));
          else
            spritesheet.texture = Texture(std::filesystem::path(spritesheet.path));

          for (auto regionChild = child->FirstChildElement("Region"); regionChild;
               regionChild = regionChild->NextSiblingElement("Region"))
          {
            Spritesheet::Region region{};
            int regionID{};

            regionChild->QueryIntAttribute("Id", &regionID);
            xml::query_string_attribute(regionChild, "Name", &region.name);
            regionChild->QueryFloatAttribute("XCrop", &region.crop.x);
            regionChild->QueryFloatAttribute("YCrop", &region.crop.y);
            regionChild->QueryFloatAttribute("Width", &region.size.x);
            regionChild->QueryFloatAttribute("Height", &region.size.y);

            if (regionChild->FindAttribute("Origin"))
            {
              std::string origin{};
              xml::query_string_attribute(regionChild, "Origin", &origin);
              region.origin = origin == "Center"    ? Spritesheet::Region::CENTER
                              : origin == "TopLeft" ? Spritesheet::Region::TOP_LEFT
                                                    : Spritesheet::Region::CUSTOM;
            }
            else
            {
              regionChild->QueryFloatAttribute("XPivot", &region.pivot.x);
              regionChild->QueryFloatAttribute("YPivot", &region.pivot.y);
            }

            spritesheet.regions.emplace(regionID, std::move(region));
            spritesheet.regionOrder.emplace_back(regionID);
          }

          spritesheets.emplace(spritesheetId, std::move(spritesheet));
        }
      }

      if (auto layersElement = contentElement->FirstChildElement("Layers"))
      {
        for (auto child = layersElement->FirstChildElement("Layer"); child; child = child->NextSiblingElement("Layer"))
        {
          int layerId{};
          Layer layer{};
          child->QueryIntAttribute("Id", &layerId);
          xml::query_string_attribute(child, "Name", &layer.name);
          child->QueryIntAttribute("SpritesheetId", &layer.spritesheetID);
          layerMap[layer.name] = layerId;
          layers.emplace(layerId, std::move(layer));
        }
      }

      if (auto nullsElement = contentElement->FirstChildElement("Nulls"))
      {
        for (auto child = nullsElement->FirstChildElement("Null"); child; child = child->NextSiblingElement("Null"))
        {
          int nullId{};
          Null null{};
          child->QueryIntAttribute("Id", &nullId);
          xml::query_string_attribute(child, "Name", &null.name);
          child->QueryBoolAttribute("ShowRect", &null.isShowRect);
          nullMap[null.name] = nullId;
          nulls.emplace(nullId, std::move(null));
        }
      }

      if (auto eventsElement = contentElement->FirstChildElement("Events"))
      {
        for (auto child = eventsElement->FirstChildElement("Event"); child; child = child->NextSiblingElement("Event"))
        {
          int eventId{};
          Event event{};
          child->QueryIntAttribute("Id", &eventId);
          xml::query_string_attribute(child, "Name", &event.name);
          eventMap[event.name] = eventId;
          events.emplace(eventId, std::move(event));
        }
      }

      if (auto soundsElement = contentElement->FirstChildElement("Sounds"))
      {
        for (auto child = soundsElement->FirstChildElement("Sound"); child; child = child->NextSiblingElement("Sound"))
        {
          int soundId{};
          Sound sound{};
          child->QueryIntAttribute("Id", &soundId);
          xml::query_string_attribute(child, "Path", &sound.path);
          if ((this->flags & NO_SOUNDS) != 0)
            sound.audio = Audio();
          else if (!archive.empty())
            sound.audio = Audio(physfs::Path(archive + "/" + sound.path));
          else
            sound.audio = Audio(std::filesystem::path(sound.path));
          sounds.emplace(soundId, std::move(sound));
        }
      }
    }

    if (auto animationsElement = element->FirstChildElement("Animations"))
    {
      xml::query_string_attribute(animationsElement, "DefaultAnimation", &defaultAnimation);

      for (auto child = animationsElement->FirstChildElement("Animation"); child;
           child = child->NextSiblingElement("Animation"))
      {
        if ((this->flags & DEFAULT_ANIMATION_ONLY) != 0)
        {
          std::string name{};
          xml::query_string_attribute(child, "Name", &name);
          if (name == defaultAnimation)
          {
            animations.emplace_back(parse_animation(child));
            break;
          }
        }
        else
          animations.emplace_back(parse_animation(child));
      }

      for (int i = 0; i < (int)animations.size(); i++)
      {
        auto& animation = animations[i];
        animationMap[animation.name] = i;
        animationMapReverse[i] = animation.name;
      }

      if (animationMap.contains(defaultAnimation))
        defaultAnimationID = animationMap[defaultAnimation];
      else
        defaultAnimationID = -1;
    }

    isValid = true;
  }

  Anm2::Anm2(const std::filesystem::path& path, Flags flags)
  {
    XMLDocument document;

    if (document.LoadFile(path.c_str()) != XML_SUCCESS)
    {
      logger.error(std::format("Failed to initialize anm2: {} ({})", path.string(), document.ErrorStr()));
      isValid = false;
      return;
    }

    WorkingDirectory workingDirectory(path, WorkingDirectory::FILE);

    this->path = path.string();

    init(document, flags);

    logger.info(std::format("Initialized anm2: {}", path.string()));
  }

  Anm2::Anm2(const physfs::Path& path, Flags flags)
  {
    XMLDocument document;

    if (xml::document_load(path, document) != XML_SUCCESS)
    {
      isValid = false;
      return;
    }

    this->path = path;
    init(document, flags, path.directory_get());

    logger.info(std::format("Initialized anm2: {}", path.c_str()));
  }

  bool Anm2::is_valid() const { return isValid; }
}
