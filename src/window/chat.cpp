#include "chat.h"

using namespace game::resource;

namespace game::window
{
  void Chat::update(Resources& resources, GameData& gameData, Text& text, Character& character)
  {

    auto size = ImGui::GetContentRegionAvail();

    ImGui::PushFont(resources.font.get(), Font::LARGE);
    if (ImGui::Button("Let's chat!", ImVec2(size.x, 0)))
    {
      resources.sound_play(audio::ADVANCE);
      text.set_random(resources.dialogue.randomIDs, resources, character);
    }
    ImGui::PopFont();
    ImGui::SetItemTooltip("Snivy will bring up a random conversation topic.");

    if (ImGui::Button("Help", ImVec2(size.x, 0)))
    {
      resources.sound_play(audio::ADVANCE);
      text.set(resources.dialogue.get("Help"), character);
    }
    ImGui::SetItemTooltip("Ask Snivy for help on this whole shebang.");
  }
}
