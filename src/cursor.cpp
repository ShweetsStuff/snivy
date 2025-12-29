#include "cursor.h"

#include "util/imgui_.h"

using namespace game::util;
using namespace game::anm2;
using namespace glm;

namespace game
{
  Cursor::Cursor(Anm2* anm2) : Actor(anm2, glm::vec2()) {}
  void Cursor::update() { position = imgui::to_vec2(ImGui::GetMousePos()); }
}