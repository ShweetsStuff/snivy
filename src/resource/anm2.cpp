#include "anm2.h"

#include <iostream>

#include "../util/xml_.h"

using namespace tinyxml2;
using namespace game::resource;
using namespace game::util;

namespace game::anm2
{
  Info::Info(XMLElement* element)
  {
    if (!element) return;
    element->QueryIntAttribute("Fps", &fps);
  }

  Spritesheet::Spritesheet(XMLElement* element, int& id)
  {
    if (!element) return;
    element->QueryIntAttribute("Id", &id);
    xml::query_path_attribute(element, "Path", &path);
    texture = Texture(path);
  }

  Layer::Layer(XMLElement* element, int& id)
  {
    if (!element) return;
    element->QueryIntAttribute("Id", &id);
    xml::query_string_attribute(element, "Name", &name);
    element->QueryIntAttribute("SpritesheetId", &spritesheetID);
  }

  Null::Null(XMLElement* element, int& id)
  {
    if (!element) return;
    element->QueryIntAttribute("Id", &id);
    xml::query_string_attribute(element, "Name", &name);
    element->QueryBoolAttribute("ShowRect", &isShowRect);
  }

  Event::Event(XMLElement* element, int& id)
  {
    if (!element) return;
    element->QueryIntAttribute("Id", &id);
    xml::query_string_attribute(element, "Name", &name);
  }

  Sound::Sound(XMLElement* element, int& id)
  {
    if (!element) return;
    element->QueryIntAttribute("Id", &id);
    xml::query_path_attribute(element, "Path", &path);
    audio = Audio(path);
  }

  Content::Content(XMLElement* element)
  {
    if (auto spritesheetsElement = element->FirstChildElement("Spritesheets"))
    {
      for (auto child = spritesheetsElement->FirstChildElement("Spritesheet"); child;
           child = child->NextSiblingElement("Spritesheet"))
      {
        int spritesheetId{};
        Spritesheet spritesheet(child, spritesheetId);
        spritesheets.emplace(spritesheetId, std::move(spritesheet));
      }
    }

    if (auto layersElement = element->FirstChildElement("Layers"))
    {
      for (auto child = layersElement->FirstChildElement("Layer"); child; child = child->NextSiblingElement("Layer"))
      {
        int layerId{};
        Layer layer(child, layerId);
        layers.emplace(layerId, std::move(layer));
      }
    }

    if (auto nullsElement = element->FirstChildElement("Nulls"))
    {
      for (auto child = nullsElement->FirstChildElement("Null"); child; child = child->NextSiblingElement("Null"))
      {
        int nullId{};
        Null null(child, nullId);
        nulls.emplace(nullId, std::move(null));
      }
    }

    if (auto eventsElement = element->FirstChildElement("Events"))
    {
      for (auto child = eventsElement->FirstChildElement("Event"); child; child = child->NextSiblingElement("Event"))
      {
        int eventId{};
        Event event(child, eventId);
        events.emplace(eventId, std::move(event));
      }
    }

    if (auto soundsElement = element->FirstChildElement("Sounds"))
    {
      for (auto child = soundsElement->FirstChildElement("Sound"); child; child = child->NextSiblingElement("Sound"))
      {
        int soundId{};
        Sound sound(child, soundId);
        sounds.emplace(soundId, std::move(sound));
      }
    }
  }

  Frame::Frame(XMLElement* element, Type type)
  {
    if (type != TRIGGER)
    {
      element->QueryFloatAttribute("XPosition", &position.x);
      element->QueryFloatAttribute("YPosition", &position.y);
      if (type == LAYER)
      {
        element->QueryFloatAttribute("XPivot", &pivot.x);
        element->QueryFloatAttribute("YPivot", &pivot.y);
        element->QueryFloatAttribute("XCrop", &crop.x);
        element->QueryFloatAttribute("YCrop", &crop.y);
        element->QueryFloatAttribute("Width", &size.x);
        element->QueryFloatAttribute("Height", &size.y);
      }
      element->QueryFloatAttribute("XScale", &scale.x);
      element->QueryFloatAttribute("YScale", &scale.y);
      element->QueryIntAttribute("Delay", &duration);
      element->QueryBoolAttribute("Visible", &isVisible);
      xml::query_color_attribute(element, "RedTint", &tint.r);
      xml::query_color_attribute(element, "GreenTint", &tint.g);
      xml::query_color_attribute(element, "BlueTint", &tint.b);
      xml::query_color_attribute(element, "AlphaTint", &tint.a);
      xml::query_color_attribute(element, "RedOffset", &colorOffset.r);
      xml::query_color_attribute(element, "GreenOffset", &colorOffset.g);
      xml::query_color_attribute(element, "BlueOffset", &colorOffset.b);
      element->QueryFloatAttribute("Rotation", &rotation);
      element->QueryBoolAttribute("Interpolated", &isInterpolated);
    }
    else
    {
      element->QueryIntAttribute("EventId", &eventID);
      element->QueryIntAttribute("SoundId", &soundID);
      element->QueryIntAttribute("AtFrame", &atFrame);
    }
  }

  Item::Item(XMLElement* element, Type type, int& id)
  {
    if (type == LAYER) element->QueryIntAttribute("LayerId", &id);
    if (type == NULL_) element->QueryIntAttribute("NullId", &id);

    element->QueryBoolAttribute("Visible", &isVisible);

    for (auto child = type == TRIGGER ? element->FirstChildElement("Trigger") : element->FirstChildElement("Frame");
         child; child = type == TRIGGER ? child->NextSiblingElement("Trigger") : child->NextSiblingElement("Frame"))
      frames.emplace_back(Frame(child, type));
  }

  Animation::Animation(XMLElement* element)
  {
    xml::query_string_attribute(element, "Name", &name);
    element->QueryIntAttribute("FrameNum", &frameNum);
    element->QueryBoolAttribute("Loop", &isLoop);

    int id{-1};

    if (auto rootAnimationElement = element->FirstChildElement("RootAnimation"))
      rootAnimation = Item(rootAnimationElement, ROOT, id);

    if (auto layerAnimationsElement = element->FirstChildElement("LayerAnimations"))
    {
      for (auto child = layerAnimationsElement->FirstChildElement("LayerAnimation"); child;
           child = child->NextSiblingElement("LayerAnimation"))
      {
        Item layerAnimation(child, LAYER, id);
        layerOrder.push_back(id);
        layerAnimations.emplace(id, std::move(layerAnimation));
      }
    }

    if (auto nullAnimationsElement = element->FirstChildElement("NullAnimations"))
    {
      for (auto child = nullAnimationsElement->FirstChildElement("NullAnimation"); child;
           child = child->NextSiblingElement("NullAnimation"))
      {
        Item nullAnimation(child, NULL_, id);
        nullAnimations.emplace(id, std::move(nullAnimation));
      }
    }

    if (auto triggersElement = element->FirstChildElement("Triggers")) triggers = Item(triggersElement, TRIGGER, id);
  }

  Animations::Animations(XMLElement* element)
  {
    xml::query_string_attribute(element, "DefaultAnimation", &defaultAnimation);

    for (auto child = element->FirstChildElement("Animation"); child; child = child->NextSiblingElement("Animation"))
      items.emplace_back(Animation(child));

    for (int i = 0; i < items.size(); i++)
    {
      auto& item = items.at(i);
      map[item.name] = i;
      mapReverse[i] = item.name;
    }
  }

  Anm2::Anm2(const std::filesystem::path& path)
  {
    XMLDocument document;

    if (document.LoadFile(path.c_str()) != XML_SUCCESS)
    {
      std::cout << "Failed to initialize anm2: " << document.ErrorStr() << "\n";
      return;
    }

    auto previousPath = std::filesystem::current_path();
    std::filesystem::current_path(path.parent_path());

    auto element = document.RootElement();

    if (auto infoElement = element->FirstChildElement("Info")) info = Info(infoElement);
    if (auto contentElement = element->FirstChildElement("Content")) content = Content(contentElement);
    if (auto animationsElement = element->FirstChildElement("Animations")) animations = Animations(animationsElement);

    std::filesystem::current_path(previousPath);

    std::cout << "Initialzed anm2: " << path.string() << "\n";
  }
}
