#pragma once

#include "resource/shader.h"

namespace game
{
  class Resources
  {
  public:
    resource::Shader shaders[resource::shader::COUNT];

    Resources();
  };
}