#include "text.h"

#include <imgui.h>

#include <algorithm>
#include <cfloat>
#include <string_view>

#include "../util/imgui_.h"

#include "../util/math_.h"

using namespace game::util;

namespace game::window
{
  constexpr auto TEXT_COLOR_DEFAULT = ImVec4(1, 1, 1, 1);
  namespace
  {
    int utf8_next_len(const char* text, const char* end)
    {
      if (text >= end) return 0;

      const unsigned char lead = static_cast<unsigned char>(*text);
      int length = 1;
      if (lead < 0x80)
        length = 1;
      else if ((lead >> 5) == 0x6)
        length = 2;
      else if ((lead >> 4) == 0xE)
        length = 3;
      else if ((lead >> 3) == 0x1E)
        length = 4;

      if (text + length > end) return 1;

      for (int i = 1; i < length; ++i)
      {
        const unsigned char byte = static_cast<unsigned char>(text[i]);
        if ((byte & 0xC0) != 0x80) return 1;
      }

      return length;
    }

    int utf8_count_chars(std::string_view text)
    {
      const char* it = text.data();
      const char* end = it + text.size();
      int count = 0;
      while (it < end)
      {
        int step = utf8_next_len(it, end);
        if (step <= 0) break;
        it += step;
        ++count;
      }
      return count;
    }

    const char* utf8_advance_chars(const char* text, const char* end, int count)
    {
      const char* it = text;
      while (it < end && count > 0)
      {
        int step = utf8_next_len(it, end);
        if (step <= 0) break;
        it += step;
        --count;
      }
      return it;
    }
  }

  void Text::set(resource::Dialogue::Entry* entry, Character& character)
  {
    if (!entry) return;
    this->entry = entry;
    this->flag = entry->flag;

    if (this->flag != resource::Dialogue::Entry::Flag::NONE) this->isFlagActivated = true;
    isFinished = false;
    index = 0;

    if (!entry->animations.empty())
    {
      for (auto& animation : entry->animations)
      {
        if (animation.at == -1)
        {
          character.play(character.animation_name_convert(animation.name));
          character.blink();
          break;
        }
      }
    }

    character.talk();
  }

  void Text::set_random(std::vector<int>& dialogueIDs, Resources& resources, Character& character)
  {
    if (dialogueIDs.empty()) return;
    set(resources.dialogue.get((dialogueIDs)[math::random_roll(dialogueIDs.size())]), character);
  }

  void Text::tick(Resources& resources, Character& character)
  {
    if (!entry || isFinished) return;

    index++;

    if (!entry->animations.empty())
    {
      for (auto& animation : entry->animations)
      {
        if (animation.at == index)
        {
          character.play(character.animation_name_convert(animation.name));
          character.blink();
          break;
        }
      }
    }

    if (index >= utf8_count_chars(entry->content)) isFinished = true;
  }

  void Text::update(Resources& resources, Character& character, ImVec2 size, ImVec2 pos)
  {
    if (!entry) return;
    auto& dialogue = resources.dialogue;
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos(pos);

    this->isFlagActivated = false;

    if (!entry) return;

    if (ImGui::Begin("##Text", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove))
    {
      auto isHovered = ImGui::IsWindowHovered();
      auto isMouse = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
      auto isSpace = ImGui::IsKeyReleased(ImGuiKey_Space);
      auto isAdvance = (isHovered && (isMouse || isSpace));

      if (ImGui::BeginTabBar("##Name"))
      {
        if (ImGui::BeginTabItem("Snivy")) ImGui::EndTabItem();
        ImGui::EndTabBar();
      }

      auto available = ImGui::GetContentRegionAvail();

      auto font = resources.font.get();
      auto fontSize = resource::Font::BIG;

      ImGui::PushFont(font, fontSize);

      auto text = [&]()
      {
        auto content = entry ? std::string_view(entry->content) : "null";
        auto length = std::clamp(index, 0, utf8_count_chars(content));

        if (length <= 0)
        {
          ImGui::Dummy(ImVec2(1.0f, ImGui::GetTextLineHeight()));
          return;
        }

        auto drawList = ImGui::GetWindowDrawList();
        auto startPos = ImGui::GetCursorScreenPos();
        auto wrapWidth = available.x <= 0.0f ? FLT_MAX : available.x;
        auto lineHeight = ImGui::GetTextLineHeightWithSpacing();
        auto cursor = startPos;
        auto maxX = startPos.x + wrapWidth;

        auto color_get = [&](int i)
        {
          if (!entry || entry->colors.empty()) return TEXT_COLOR_DEFAULT;

          for (auto& color : entry->colors)
            if (i >= color.start && i <= color.end)
              return ImVec4(color.value.r, color.value.g, color.value.b, color.value.a);

          return TEXT_COLOR_DEFAULT;
        };

        const char* textStart = content.data();
        const char* textEnd = textStart + content.size();
        const char* textLimit = utf8_advance_chars(textStart, textEnd, length);

        int i = 0;
        for (const char* it = textStart; it < textLimit;)
        {
          int step = utf8_next_len(it, textLimit);
          if (step <= 0) break;

          if (*it == '\n')
          {
            cursor.x = startPos.x;
            cursor.y += lineHeight;
            it += step;
            ++i;
            continue;
          }

          if (*it == ' ')
          {
            auto glyphSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, it, it + step);
            if (cursor.x != startPos.x && cursor.x + glyphSize.x > maxX)
            {
              cursor.x = startPos.x;
              cursor.y += lineHeight;
              it += step;
              ++i;
              continue;
            }

            if (cursor.x == startPos.x)
            {
              it += step;
              ++i;
              continue;
            }

            drawList->AddText(font, fontSize, cursor, ImGui::GetColorU32(color_get(i)), it, it + step);
            cursor.x += glyphSize.x;
            it += step;
            ++i;
            continue;
          }

          const char* wordStart = it;
          const char* wordEnd = it;
          float wordWidth = 0.0f;
          int wordChars = 0;

          for (const char* wordIt = it; wordIt < textLimit;)
          {
            int wordStep = utf8_next_len(wordIt, textLimit);
            if (wordStep <= 0) break;
            if (*wordIt == '\n' || *wordIt == ' ') break;

            auto glyphSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, wordIt, wordIt + wordStep);
            wordWidth += glyphSize.x;
            wordIt += wordStep;
            ++wordChars;
            wordEnd = wordIt;
          }

          if (cursor.x != startPos.x && cursor.x + wordWidth > maxX)
          {
            cursor.x = startPos.x;
            cursor.y += lineHeight;
          }

          for (const char* wordIt = wordStart; wordIt < wordEnd;)
          {
            int wordStep = utf8_next_len(wordIt, wordEnd);
            if (wordStep <= 0) break;

            auto glyphSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, wordIt, wordIt + wordStep);
            drawList->AddText(font, fontSize, cursor, ImGui::GetColorU32(color_get(i)), wordIt, wordIt + wordStep);
            cursor.x += glyphSize.x;
            wordIt += wordStep;
            ++i;
          }

          it = wordEnd;
        }

        float layoutWidth = wrapWidth == FLT_MAX ? (cursor.x - startPos.x) : wrapWidth;
        if (layoutWidth <= 0.0f) layoutWidth = 1.0f;
        auto totalHeight = (cursor.y - startPos.y) + lineHeight;
        ImGui::Dummy(ImVec2(layoutWidth, totalHeight));
      };

      text();

      if (entry)
      {
        if (isFinished)
        {
          character.talkOverride.isLoop = false;

          if (!entry->branches.empty())
          {
            ImGui::SetCursorPos(ImVec2(ImGui::GetStyle().WindowPadding.x, available.y));
            auto buttonSize = imgui::widget_size_with_row_get(entry->branches.size());

            for (auto& branch : entry->branches)
            {
              if (ImGui::Button(branch.content.c_str(), buttonSize))
              {
                set(dialogue.get(branch.nextID), character);
                resources.sound_play(audio::ADVANCE);
              }
              ImGui::SameLine();
            }

            if (isHovered && isSpace)
            {
              set(dialogue.get(entry->branches.front().nextID), character);
              resources.sound_play(audio::ADVANCE);
            }
          }
          else
          {
            if (entry->nextID != -1)
            {
              ImGui::SetCursorPos(ImVec2(available.x - ImGui::GetTextLineHeightWithSpacing(),
                                         available.y - ImGui::GetStyle().WindowPadding.y));
              ImGui::Text("▶");

              if (isAdvance)
              {
                resources.sound_play(audio::ADVANCE);
                set(dialogue.get(entry->nextID), character);
              }
            }
          }
        }
        else
        {
          if (isAdvance)
          {
            index = utf8_count_chars(entry->content);
            isFinished = true;

            if (!entry->animations.empty())
            {
              auto& animation = entry->animations.back();
              auto name = character.animation_name_convert(animation.name);
              if (auto animationIndex = character.animation_index_get(name); animationIndex != character.animationIndex)
              {
                character.play(name);
                character.blink();
              }
            }
          }
        }
      }

      ImGui::PopFont();
    };
    ImGui::End();
  }
}
