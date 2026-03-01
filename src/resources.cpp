#include "resources.hpp"

#include "util/preferences.hpp"
#include "log.hpp"

using namespace game::resource;
using namespace game::util;

namespace game
{
  Resources::Resources()
  {
    for (int i = 0; i < shader::COUNT; i++)
      shaders[i] = Shader(shader::INFO[i].vertex, shader::INFO[i].fragment);

    std::string CHARACTERS_DIRECTORY{"resources/characters"}; 
    std::error_code ec{};

    if (!std::filesystem::is_directory(CHARACTERS_DIRECTORY, ec))
    {
      if (ec)
        logger.warning("Failed to read characters directory: " + CHARACTERS_DIRECTORY + " (" + ec.message() + ")");
      else
        logger.warning("Characters directory not found: " + CHARACTERS_DIRECTORY);
      return;
    }

    for (auto& entry : std::filesystem::recursive_directory_iterator(CHARACTERS_DIRECTORY))
      if (entry.is_regular_file() && entry.path().extension() == ".zip") characterPreviews.emplace_back(entry.path());
    characters.resize(characterPreviews.size());
  }

  void Resources::volume_set(float vol) { Audio::volume_set(vol); }

  resource::xml::Character& Resources::character_get(int index)
  {
    if (!characters.at(index).has_value())
    {
      characters[index].emplace(characterPreviews.at(index).path);
      characters[index]->save = characterPreviews.at(index).save;
    }
    return *characters[index];
  }

  resource::xml::CharacterPreview& Resources::character_preview_get(int index) { return characterPreviews.at(index); }

  void Resources::character_save_set(int index, const resource::xml::Save& save)
  {
    characterPreviews.at(index).save = save;
    if (characters.at(index).has_value()) characters[index]->save = save;
  }

  Resources::~Resources() { settings.serialize(preferences::path() / "settings.xml"); }
}
