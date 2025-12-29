#include "font.h"

namespace game::resource
{
  Font::Font(const std::string& path, float size)
  {
    internal = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str(), size);
  }
  ImFont* Font::get() { return internal; };
}