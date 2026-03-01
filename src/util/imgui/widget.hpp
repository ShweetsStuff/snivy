#pragma once

#include "../../resource/xml/sound_entry.hpp"
#include <imgui.h>

namespace game::util::imgui::widget
{
  void sounds_set(resource::xml::SoundEntryCollection* _hover, resource::xml::SoundEntryCollection* _select);
  void fx();
}

#define WIDGET_FX_LAST_ITEM() ::game::util::imgui::widget::fx()
#define WIDGET_FX(expr)                                                                                                \
  (                                                                                                                    \
      [&]()                                                                                                            \
      {                                                                                                                \
        const auto _widget_result = (expr);                                                                            \
        WIDGET_FX_LAST_ITEM();                                                                                         \
        return _widget_result;                                                                                         \
      }())
