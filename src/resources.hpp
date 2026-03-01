#pragma once

#include <optional>

#include "resource/font.hpp"
#include "resource/shader.hpp"
#include "resource/xml/character.hpp"
#include "resource/xml/character_preview.hpp"
#include "resource/xml/settings.hpp"

namespace game
{
  class Resources
  {
  public:
    resource::Shader shaders[resource::shader::COUNT];
    resource::Font font{"resources/font/font.ttf"};
    resource::xml::Settings settings;

    std::vector<resource::xml::CharacterPreview> characterPreviews{};
    std::vector<std::optional<resource::xml::Character>> characters{};

    Resources();
    ~Resources();

    resource::xml::Character& character_get(int index);
    resource::xml::CharacterPreview& character_preview_get(int index);
    void character_save_set(int index, const resource::xml::Save& save);

    void volume_set(float volume);
  };
}
