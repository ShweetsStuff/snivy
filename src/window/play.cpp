#include "play.h"

#include "../util/imgui_.h"
#include "../util/math_.h"
#include <format>

using namespace game::util;
using namespace glm;

namespace game::window
{
  const std::unordered_map<Play::Grade, audio::Type> GRADE_SOUNDS = {
      {Play::MISS, audio::MISS},           {Play::OK, audio::OK},
      {Play::GOOD, audio::GOOD},           {Play::GREAT, audio::GREAT},
      {Play::EXCELLENT, audio::EXCELLENT}, {Play::PERFECT, audio::PERFECT}};

  const std::unordered_map<Item::Rarity, audio::Type> RARITY_SOUNDS = {{Item::COMMON, audio::COMMON},
                                                                       {Item::UNCOMMON, audio::UNCOMMON},
                                                                       {Item::RARE, audio::RARE},
                                                                       {Item::EPIC, audio::EPIC},
                                                                       {Item::LEGENDARY, audio::LEGENDARY},
                                                                       {Item::SPECIAL, audio::SPECIAL},
                                                                       {Item::IMPOSSIBLE, audio::IMPOSSIBLE}};

  float Play::accuracy_score_get()
  {
    if (totalPlays == 0) return 0.0f;
    float combinedWeight{};
    for (int i = 0; i < Play::GRADE_COUNT; i++)
      combinedWeight += (GRADE_WEIGHTS[(Play::Grade)i] * gradeCounts[(Play::Grade)i]);

    return glm::clamp(0.0f, math::to_percent(combinedWeight / totalPlays), 100.0f);
  }

  Play::Challenge Play::challenge_generate(int level)
  {
    level = std::max(1, level);

    Challenge newChallenge;

    newChallenge.level = level;

    Range newRange{};

    auto rangeSize = std::max(RANGE_MIN, RANGE_BASE - (RANGE_SCORE_BONUS * score));
    newRange.min = math::random_in_range(0.0, 1.0f - rangeSize);
    newRange.max = newRange.min + rangeSize;

    newChallenge.range = newRange;
    newChallenge.tryValue = 0.0f;

    newChallenge.speed = glm::clamp(SPEED_BASE, SPEED_BASE + (SPEED_SCORE_BONUS * score), SPEED_MAX);

    if (math::random_bool())
    {
      newChallenge.tryValue = 1.0f;
      newChallenge.speed *= -1;
    }

    return newChallenge;
  }

  Play::Play()
  {
    challenge = challenge_generate(1);

    for (int i = 0; i < Play::GRADE_COUNT; i++)
      gradeCounts[(Play::Grade)i] = 0;
  }

  void Play::update(Resources& resources, Character& character, Inventory& inventory, GameData& gameData, Text& text)
  {
    auto drawList = ImGui::GetWindowDrawList();

    auto size = ImGui::GetContentRegionAvail();
    auto position = ImGui::GetCursorScreenPos();
    auto spacing = ImGui::GetTextLineHeightWithSpacing();
    auto level = character.weightStage + 1;

    ImGui::Text("Score: %i pts (%ix)", score, combo);
    ImGui::Text("Best: %i pts (%ix)", highScore, comboBest);
    if (score == 0 && isActive)
      ImGui::Text("Match the line to the\ncolored areas with Space/click!\nBetter performance, better rewards!");

    auto barMin = ImVec2(position.x + (size.x * 0.5f) - (spacing * 0.5f), position.y + (spacing * 2.0f));
    auto barMax = ImVec2(barMin.x + (spacing * 2.0f), barMin.y + size.y - (spacing * 4.0f));

    bool mouseHovering = ImGui::IsMouseHoveringRect(barMin, barMax);

    if (mouseHovering)
    {
      drawList->AddRect(ImVec2(barMin.x - 1, barMin.y - 1), ImVec2(barMax.x + 1, barMax.y + 1),
                        ImGui::GetColorU32(RECT_COLOR));
    }

    drawList->AddRectFilled(barMin, barMax, ImGui::GetColorU32(BG_COLOR));

    auto barWidth = barMax.x - barMin.x;
    auto barHeight = barMax.y - barMin.y;

    auto sub_ranges_get = [&](Range& range)
    {
      auto& min = range.min;
      auto& max = range.max;
      auto baseMinY = min;
      auto baseMaxY = max;
      std::vector<Range> ranges{};

      auto baseHeight = max - min;
      auto center = (min + max) * 0.5f;

      for (int i = 0; i < RANGE_AREA_COUNT; ++i)
      {
        auto scale = powf(0.5f, i);
        auto halfHeight = baseHeight * scale * 0.5f;

        ranges.push_back({center - halfHeight, center + halfHeight});
      }

      return ranges;
    };

    auto range_draw = [&](Range& range, float alpha = 1.0f)
    {
      auto subRanges = sub_ranges_get(range);

      for (int i = 0; i < (int)subRanges.size(); i++)
      {
        auto& subRange = subRanges[i];
        int layer = (int)subRanges.size() - 1 - i;

        ImVec2 rectMin = {barMin.x, barMin.y + subRange.min * barHeight};

        ImVec2 rectMax = {barMax.x, barMin.y + subRange.max * barHeight};

        ImVec4 color = i == subRanges.size() - 1 ? PERFECT_COLOR : RECT_COLOR;
        color.w = (color.w - (float)layer / subRanges.size()) * alpha;

        drawList->AddRectFilled(rectMin, rectMax, ImGui::GetColorU32(color));
      }
    };

    auto endTimerProgress = (float)endTimer / endTimerMax;

    range_draw(challenge.range, isActive ? 1.0f : 0.0f);

    auto lineMin = ImVec2(barMin.x - LINE_WIDTH_BONUS, barMin.y + (barHeight * tryValue));
    auto lineMax = ImVec2(barMin.x + barWidth + LINE_WIDTH_BONUS, lineMin.y + LINE_HEIGHT);
    auto color = LINE_COLOR;
    color.w = isActive ? 1.0f : endTimerProgress;
    drawList->AddRectFilled(lineMin, lineMax, ImGui::GetColorU32(color));

    if (!isActive && !isGameOver)
    {
      range_draw(queuedChallenge.range, 1.0f - endTimerProgress);

      auto lineMin = ImVec2(barMin.x - LINE_WIDTH_BONUS, barMin.y + (barHeight * queuedChallenge.tryValue));
      auto lineMax = ImVec2(barMin.x + barWidth + LINE_WIDTH_BONUS, lineMin.y + LINE_HEIGHT);
      auto color = LINE_COLOR;
      color.w = 1.0f - endTimerProgress;
      drawList->AddRectFilled(lineMin, lineMax, ImGui::GetColorU32(color));
    }

    if (isActive)
    {
      tryValue += challenge.speed;

      if (tryValue > 1.0f || tryValue < 0.0f)
      {
        tryValue = tryValue > 1.0f ? 0.0f : tryValue < 0.0f ? 1.0f : tryValue;

        if (score > 0)
        {
          score--;
          resources.sound_play(audio::SCORE_LOSS);
          auto toastMessagePosition =
              ImVec2(barMin.x - ImGui::CalcTextSize("-1").x - ImGui::GetTextLineHeightWithSpacing(), lineMin.y);
          toastMessages.emplace_back("-1", toastMessagePosition, END_TIMER_MAX, END_TIMER_MAX);
        }
      }

      if (ImGui::IsKeyPressed(ImGuiKey_Space) || (mouseHovering && ImGui::IsMouseClicked(ImGuiMouseButton_Left)))
      {
        Grade grade{MISS};
        auto subRanges = sub_ranges_get(challenge.range);

        for (int i = 0; i < (int)subRanges.size(); i++)
        {
          auto& subRange = subRanges[i];

          if (tryValue >= subRange.min && tryValue <= subRange.max)
            grade = (Grade)std::min((int)(grade + 1), (int)PERFECT);
        }

        gradeCounts[grade]++;
        totalPlays++;

        if (grade != MISS)
        {
          if (grade == PERFECT) text.set_random(resources.dialogue.perfectIDs, resources, character);

          combo++;
          score += (int)grade;
          if (score > highScore)
          {
            highScore = score;

            if (highScore >= HIGH_SCORE_BIG && !isHighScoreBigAchieved)
            {
              resources.sound_play(audio::HIGH_SCORE_BIG);
              isHighScoreBigAchieved = true;

              inventory.values[Item::POKE_PUFF_SUPREME_HONOR]++;

              auto toastItemPosition =
                  ImVec2(math::random_in_range(barMax.x + ITEM_SIZE.x, barMax.x + (size.x * 0.5f) - ITEM_SIZE.x),
                         position.y - math::random_in_range(ITEM_SIZE.y, ITEM_SIZE.y * 2.0f));
              toastItems.emplace_back(Item::POKE_PUFF_SUPREME_HONOR, toastItemPosition);

              auto toastMessagePosition =
                  ImVec2(barMin.x - ImGui::CalcTextSize("Fantastic score! Congratulations!").x -
                             ImGui::GetTextLineHeightWithSpacing(),
                         lineMin.y + (ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().ItemSpacing.y));
              toastMessages.emplace_back("Fantastic score! Congratulations!", toastMessagePosition, END_TIMER_MAX,
                                         END_TIMER_MAX);
            }

            if (highScoreStart > 0)
            {
              if (!isHighScoreAchieved)
              {
                resources.sound_play(audio::HIGH_SCORE);
                isHighScoreAchieved = true;
                auto toastMessagePosition =
                    ImVec2(barMin.x - ImGui::CalcTextSize("High Score!").x - ImGui::GetTextLineHeightWithSpacing(),
                           lineMin.y + ImGui::GetTextLineHeightWithSpacing());
                toastMessages.emplace_back("High Score!", toastMessagePosition, END_TIMER_MAX, END_TIMER_MAX);
              }
            }
          }

          if (combo > comboBest) comboBest = combo;

          auto rewardBonus =
              (REWARD_SCORE_BONUS * score) + (REWARD_LEVEL_BONUS * level) + (REWARD_GRADE_BONUS * (int)grade);
          while (rewardBonus > 0.0f)
          {
            const Item::Pool* pool{};
            auto rewardType = Item::NONE;
            auto gradeChanceBonus = REWARD_GRADE_CHANCE_BONUS * (int)grade;
            auto levelChanceBonus = REWARD_LEVEL_CHANCE_BONUS * (int)level;
            auto chanceBonus = std::max(1.0f, gradeChanceBonus + levelChanceBonus);
            if (math::random_percent_roll(Item::RARITY_CHANCES[Item::IMPOSSIBLE] * chanceBonus))
              pool = &Item::pools[Item::IMPOSSIBLE];
            else if (math::random_percent_roll(Item::RARITY_CHANCES[Item::LEGENDARY] * chanceBonus))
              pool = &Item::pools[Item::LEGENDARY];
            else if (math::random_percent_roll(Item::RARITY_CHANCES[Item::EPIC] * chanceBonus))
              pool = &Item::pools[Item::EPIC];
            else if (math::random_percent_roll(Item::RARITY_CHANCES[Item::RARE] * chanceBonus))
              pool = &Item::pools[Item::RARE];
            else if (math::random_percent_roll(Item::RARITY_CHANCES[Item::UNCOMMON] * chanceBonus))
              pool = &Item::pools[Item::UNCOMMON];
            else if (math::random_percent_roll(Item::RARITY_CHANCES[Item::COMMON] * chanceBonus))
              pool = &Item::pools[Item::COMMON];

            if (pool && !pool->empty())
            {
              rewardType = (*pool)[(int)math::random_roll((float)pool->size())];

              auto& rarity = Item::RARITIES[rewardType];

              resources.sound_play(audio::FALL);
              resources.sound_play(RARITY_SOUNDS.at(rarity));

              inventory.values[rewardType]++;

              auto toastItemPosition =
                  ImVec2(math::random_in_range(barMax.x + ITEM_SIZE.x, barMax.x + (size.x * 0.5f) - ITEM_SIZE.x),
                         position.y - math::random_in_range(ITEM_SIZE.y, ITEM_SIZE.y * 2.0f));
              toastItems.emplace_back(rewardType, toastItemPosition);
            }

            rewardBonus -= 1.0f;
          }
        }
        else
        {
          text.set_random(resources.dialogue.missIDs, resources, character);
          score = 0;
          if (isHighScoreAchieved) resources.sound_play(audio::HIGH_SCORE_LOSS);
          isHighScoreAchieved = false;
          highScoreStart = highScore;
          isGameOver = true;
        }
        resources.sound_play(GRADE_SOUNDS.at(grade));

        endTimerMax = grade == MISS ? END_TIMER_MISS_MAX : END_TIMER_MAX;
        isActive = false;
        endTimer = endTimerMax;

        queuedChallenge = challenge_generate(level);

        auto string =
            grade == MISS ? GRADE_STRINGS[grade] : std::format("{} (+{})", GRADE_STRINGS[grade], GRADE_VALUES[grade]);
        auto toastMessagePosition =
            ImVec2(barMin.x - ImGui::CalcTextSize(string.c_str()).x - ImGui::GetTextLineHeightWithSpacing(), lineMin.y);
        toastMessages.emplace_back(string, toastMessagePosition, endTimerMax, endTimerMax);
      }
    }
    else
    {
      endTimer--;
      if (endTimer <= 0)
      {
        challenge = queuedChallenge;
        tryValue = challenge.tryValue;
        isActive = true;
        isGameOver = false;
      }
    }

    for (int i = 0; i < (int)toastMessages.size(); i++)
    {
      auto& toastMessage = toastMessages[i];

      toastMessage.position.y -= TOAST_MESSAGE_SPEED;

      auto color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
      color.w = ((float)toastMessage.time / toastMessage.timeMax);

      drawList->AddText(toastMessage.position, ImGui::GetColorU32(color), toastMessage.message.c_str());

      toastMessage.time--;

      if (toastMessage.time <= 0) toastMessages.erase(toastMessages.begin() + i--);
    }

    for (int i = 0; i < (int)toastItems.size(); i++)
    {
      auto& texture = resources.anm2s[anm2::ITEMS].content.spritesheets.at(0).texture;
      auto& toastItem = toastItems[i];
      auto& type = toastItem.type;

      auto columns = (int)(texture.size.x / ITEM_SIZE.x);
      auto crop = vec2(type % columns, type / columns) * ITEM_SIZE;
      auto uvMin = imgui::to_imvec2(crop / vec2(texture.size));
      auto uvMax = imgui::to_imvec2((crop + ITEM_SIZE) / vec2(texture.size));

      auto min = ImVec2(toastItem.position.x - (ITEM_SIZE.x * 0.5f), toastItem.position.y - (ITEM_SIZE.y * 0.5f));
      auto max = ImVec2(toastItem.position.x + (ITEM_SIZE.x * 0.5f), toastItem.position.y + (ITEM_SIZE.y * 0.5f));

      drawList->AddImage(texture.id, min, max, uvMin, uvMax);

      toastItem.velocityY += Item::GRAVITY;
      toastItem.position.y += toastItem.velocityY;
      if (toastItem.position.y > position.y + size.y + ITEM_SIZE.y) toastItems.erase(toastItems.begin() + i--);
    }
  }
}