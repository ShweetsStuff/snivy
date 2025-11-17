#include "anm2.h"
#include <iostream>

using namespace tinyxml2;
using namespace game::resource;

namespace game::anm2
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

  Info::Info(XMLElement* element)
  {
    if (!element) return;
    element->QueryIntAttribute("Fps", &fps);
  }

  Spritesheet::Spritesheet(XMLElement* element, int& id, TextureCallback textureCallback)
  {
    if (!element) return;
    element->QueryIntAttribute("Id", &id);
    query_path_attribute(element, "Path", &path);
    texture = Texture(path);
  }

  Layer::Layer(XMLElement* element, int& id)
  {
    if (!element) return;
    element->QueryIntAttribute("Id", &id);
    query_string_attribute(element, "Name", &name);
    element->QueryIntAttribute("SpritesheetId", &spritesheetID);
  }

  Null::Null(XMLElement* element, int& id)
  {
    if (!element) return;
    element->QueryIntAttribute("Id", &id);
    query_string_attribute(element, "Name", &name);
    element->QueryBoolAttribute("ShowRect", &isShowRect);
  }

  Event::Event(XMLElement* element, int& id)
  {
    if (!element) return;
    element->QueryIntAttribute("Id", &id);
    query_string_attribute(element, "Name", &name);
  }

  Sound::Sound(XMLElement* element, int& id, SoundCallback soundCallback)
  {
    if (!element) return;
    element->QueryIntAttribute("Id", &id);
    query_path_attribute(element, "Path", &path);
    audio = Audio(path);
  }

  Content::Content(XMLElement* element, TextureCallback textureCallback, SoundCallback soundCallback)
  {
    if (auto spritesheetsElement = element->FirstChildElement("Spritesheets"))
    {
      for (auto child = spritesheetsElement->FirstChildElement("Spritesheet"); child;
           child = child->NextSiblingElement("Spritesheet"))
      {
        int spritesheetId{};
        Spritesheet spritesheet(child, spritesheetId, textureCallback);
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
        Sound sound(child, soundId, soundCallback);
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
      query_color_attribute(element, "RedTint", &tint.r);
      query_color_attribute(element, "GreenTint", &tint.g);
      query_color_attribute(element, "BlueTint", &tint.b);
      query_color_attribute(element, "AlphaTint", &tint.a);
      query_color_attribute(element, "RedOffset", &colorOffset.r);
      query_color_attribute(element, "GreenOffset", &colorOffset.g);
      query_color_attribute(element, "BlueOffset", &colorOffset.b);
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
    query_string_attribute(element, "Name", &name);
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
    query_string_attribute(element, "DefaultAnimation", &defaultAnimation);

    for (auto child = element->FirstChildElement("Animation"); child; child = child->NextSiblingElement("Animation"))
      items.emplace_back(Animation(child));
  }

  Anm2::Anm2(const std::filesystem::path& path, TextureCallback textureCallback, SoundCallback soundCallback)
  {
    XMLDocument document;

    if (document.LoadFile(path.c_str()) != XML_SUCCESS)
    {
      std::cout << "Failed to initialize anm2: " << document.ErrorStr() << "\n";
      return;
    }

    std::cout << "Initialzed anm2: " << path.string() << "\n";

    auto previousPath = std::filesystem::current_path();
    std::filesystem::current_path(path.parent_path());

    auto element = document.RootElement();

    if (auto infoElement = element->FirstChildElement("Info")) info = Info(infoElement);
    if (auto contentElement = element->FirstChildElement("Content"))
      content = Content(contentElement, textureCallback, soundCallback);
    if (auto animationsElement = element->FirstChildElement("Animations")) animations = Animations(animationsElement);

    std::filesystem::current_path(previousPath);
  }
}
