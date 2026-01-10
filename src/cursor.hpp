#pragma once

#include "resource/actor.h"

namespace game
{
  class Cursor : public resource::Actor
  {

  public:
    static constexpr const char* ANIMATION_DEFAULT = "Default";
    static constexpr const char* ANIMATION_HOVER = "Hover";
    static constexpr const char* ANIMATION_GRAB = "Grab";
    static constexpr const char* ANIMATION_RUB = "Rub";

    Cursor(anm2::Anm2* anm2);
    void update();
  };
}