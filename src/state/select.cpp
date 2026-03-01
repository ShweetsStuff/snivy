#include "select.hpp"

#include <imgui_impl_opengl3.h>

using namespace game::util;

namespace game::state
{
  void Select::tick() { preview.tick(characterIndex); }

  void Select::update(Resources& resources)
  {
    preview.update(resources, characterIndex);
    info.update(resources, characterIndex);
    characters.update(resources, characterIndex);
  }

  void Select::render(Resources&, Canvas& canvas)
  {
    canvas.bind();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    canvas.unbind();
  }
};
