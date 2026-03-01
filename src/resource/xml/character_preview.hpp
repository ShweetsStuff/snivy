#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "anm2.hpp"
#include "save.hpp"

namespace game::resource::xml
{
  class CharacterPreview
  {
  public:
    struct Stage
    {
      float threshold{};
      int dialoguePoolID{-1};
    };

    Anm2 anm2{};
    Texture portrait{};
    Texture render{};
    Save save{};

    int stages{1};

    std::string name{};
    std::string author{};
    std::string description{};
    std::filesystem::path path{};
    float weight{50};

    bool isValid{};

    CharacterPreview() = default;
    CharacterPreview(const std::filesystem::path&);

    std::filesystem::path save_path_get();
    bool is_valid() const;
  };
}
