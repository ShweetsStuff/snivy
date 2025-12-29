#include "resources.h"

using namespace game::resource;
using namespace game::anm2;

namespace game
{
  Resources::Resources()
  {
    for (int i = 0; i < shader::COUNT; i++)
      shaders[i] = Shader(shader::INFO[i].vertex, shader::INFO[i].fragment);

    for (int i = 0; i < audio::COUNT; i++)
      audio[i] = Audio(audio::PATHS[i]);

    for (int i = 0; i < texture::COUNT; i++)
      textures[i] = Texture(texture::PATHS[i]);

    for (int i = 0; i < anm2::COUNT; i++)
      anm2s[i] = Anm2(anm2::PATHS[i]);
  }

  void Resources::sound_play(audio::Type type) { audio[type].play(); }
}