#include "resources.h"

using namespace game::resource;

namespace game
{
  Resources::Resources()
  {
    for (int i = 0; i < shader::COUNT; i++)
      shaders[i] = Shader(shader::INFO[i].vertex, shader::INFO[i].fragment);
  }
}