#include "cursor.hpp"

#include "../util/imgui.hpp"

using namespace game::util;
using namespace glm;

namespace game::entity
{
  Cursor::Cursor(Anm2& anm2) : Actor(anm2, imgui::to_vec2(ImGui::GetMousePos())) {}
  void Cursor::tick()
  {
    Actor::tick();
    queue_default_animation();
  }

  void Cursor::update()
  {
    state = DEFAULT;
    position = imgui::to_vec2(ImGui::GetMousePos());
  }
}
