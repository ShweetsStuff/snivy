#include "state.h"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

#include "util/math_.h"

using namespace glm;
using namespace game::resource;
using namespace game::util;

namespace game
{
  constexpr auto TICK_RATE = 30;
  constexpr auto TICK_INTERVAL = (1000 / TICK_RATE);
  constexpr auto UPDATE_RATE = 60;
  constexpr auto UPDATE_INTERVAL = (1000 / UPDATE_RATE);

  State::State(SDL_Window* inWindow, SDL_GLContext inContext, vec2 size)
      : window(inWindow), context(inContext), canvas(size, true)
  {
  }

  void State::tick()
  {
    for (auto& item : items)
      item.tick();

    character.tick();

    if (character.isJustDigestionStart)
      resources.sound_play(audio::GURGLES[(int)math::random_roll(std::size(audio::GURGLES))]);
    if (character.is_event("Burp")) resources.sound_play(audio::BURPS[(int)math::random_roll(std::size(audio::BURPS))]);
    if (character.isJustDigestionEnd && !character.isJustStageUp)
    {
      character.state_set(Character::PAT, true);
      textWindow.set_random(resources.dialogue.postDigestIDs, resources, character);
    }

    cursor.tick();

    textWindow.tick(resources, character);
  }

  void State::update()
  {
    static bool isRubbing{};
    auto& inventory = mainMenuWindow.inventory;
    auto& dialogue = resources.dialogue;

    int width{};
    int height{};
    SDL_GetWindowSize(window, &width, &height);

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT) isRunning = false;
    }
    if (!isRunning) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    auto style = ImGui::GetStyle();

    if (textWindow.isFlagActivated)
    {
      switch (textWindow.flag)
      {
        case Dialogue::Entry::ACTIVATE_WINDOWS:
          isInfo = true;
          isMainMenu = true;
          break;
        case Dialogue::Entry::DEACTIVATE_WINDOWS:
          isInfo = false;
          isMainMenu = false;
          break;
        case Dialogue::Entry::ONLY_INFO:
          isInfo = true;
          isMainMenu = false;
          break;
        case Dialogue::Entry::ACTIVATE_CHEATS:
          mainMenuWindow.isCheats = true;
          break;
        default:
          break;
      }
    }

    auto infoSize = ImVec2(width * 0.5f, ImGui::GetTextLineHeightWithSpacing() * 3.5);
    auto infoPos = style.WindowPadding;
    if (isInfo) infoWindow.update(resources, gameData, character, infoSize, infoPos);

    auto textSize = ImVec2(width - style.WindowPadding.x * 2, ImGui::GetTextLineHeightWithSpacing() * 8);
    auto textPos = ImVec2(style.WindowPadding.x, height - textSize.y - style.WindowPadding.y);
    if (isText) textWindow.update(resources, character, textSize, textPos);

    auto mainSize =
        ImVec2((width * 0.5f) - style.WindowPadding.x * 3, height - textSize.y - (style.WindowPadding.y * 3));
    auto mainPos = ImVec2(infoPos.x + infoSize.x + style.WindowPadding.x, style.WindowPadding.y);
    if (isMainMenu) mainMenuWindow.update(resources, character, gameData, textWindow, mainSize, mainPos);

    /* Inventory */

    if (inventory.isQueued)
    {
      if (items.size() > Item::DEPLOYED_MAX)
      {
        inventory.isQueued = false;
        inventory.queuedItemType = Item::NONE;
        resources.sound_play(audio::MISS);
      }
      else
      {
        auto& type = inventory.queuedItemType;

        auto position = glm::vec2(((Item::SPAWN_X_MAX - Item::SPAWN_X_MIN) * math::random()) + Item::SPAWN_X_MIN,
                                  ((Item::SPAWN_Y_MAX - Item::SPAWN_Y_MIN) * math::random()) + Item::SPAWN_Y_MIN);
        items.emplace_back(&resources.anm2s[anm2::ITEMS], position, type);

        inventory.adjust_item(type, -1);
        type = Item::NONE;
        inventory.isQueued = false;
      }
    }

    /* Item */
    Item::hoveredItem = nullptr;
    Item::heldItemPrevious = Item::heldItem;
    bool isSpeak{};
    std::vector<int>* dialogueIDs{};

    for (int i = 0; i < items.size(); i++)
    {
      auto& item = items[i];

      item.update(resources);

      if (&item == Item::heldItem && &item != &items.back())
      {
        std::swap(items[i], items.back());
        Item::heldItem = &items.back();
        continue;
      }

      if (item.isToBeDeleted)
      {
        items.erase(items.begin() + i--);
        continue;
      }

      if (Item::queuedReturnItem == &item)
      {
        if (Item::queuedReturnItem->state == Item::DEFAULT)
        {
          inventory.adjust_item(item.type);
          resources.sound_play(audio::RETURN);
        }
        else
          resources.sound_play(audio::DISPOSE);

        items.erase(items.begin() + i--);
        Item::queuedReturnItem = nullptr;
        continue;
      }
    }

    auto& item = Item::heldItem;
    auto isHeldItemChanged = item != Item::heldItemPrevious;

    if (item && character.state != Character::STAGE_UP)
    {
      auto& type = item->type;
      auto& calories = Item::CALORIES[type];
      auto& category = Item::CATEGORIES[type];
      auto& position = item->position;
      auto caloriesChew = Item::CALORIES[type] / (Item::CHEW_COUNT_MAX + 1);
      auto digestionRateBonusChew = Item::DIGESTION_RATE_BONUSES[type] / (Item::CHEW_COUNT_MAX + 1);
      auto eatSpeedBonusChew = Item::EAT_SPEED_BONUSES[type] / (Item::CHEW_COUNT_MAX + 1);
      auto isAbleToEat = character.calories + caloriesChew <= character.max_capacity();

      if (category == Item::FOOD)
      {
        auto isByMouth = math::is_point_in_rectf(character.mouth_rect_get(), position);

        if (character.state == Character::EAT && !isHeldItemChanged)
        {
          if (!isByMouth)
          {
            if (character.is_over_capacity())
            {
              dialogueIDs = &dialogue.foodEasedIDs;
              character.state_set(Character::IDLE);
            }
            else
            {
              dialogueIDs = &dialogue.foodStolenIDs;
              character.state_set(Character::ANGRY);
            }
            isSpeak = true;
          }

          if (character.is_event(Character::EVENT_EAT))
          {
            item->chewCount++;

            if (digestionRateBonusChew > 0 || digestionRateBonusChew < 0)
            {
              character.digestionRate =
                  glm::clamp(Character::DIGESTION_RATE_MIN, character.digestionRate + digestionRateBonusChew,
                             Character::DIGESTION_RATE_MAX);
            }

            if (eatSpeedBonusChew > 0)
            {
              character.eatSpeedMultiplier =
                  glm::clamp(Character::EAT_SPEED_MULTIPLIER_MIN, character.eatSpeedMultiplier + eatSpeedBonusChew,
                             Character::EAT_SPEED_MULTIPLIER_MAX);
            }

            character.calories += caloriesChew;
            character.totalCaloriesConsumed += caloriesChew;
            character.consume_played_event();

            if (item->chewCount > Item::CHEW_COUNT_MAX)
            {
              character.isFinishedFood = true;
              character.foodItemsEaten++;

              item->isToBeDeleted = true;
              Item::heldItem = nullptr;
              cursor.play(Cursor::ANIMATION_DEFAULT);
            }
            else
              item->state_set(item->chewCount == 1   ? Item::CHEW_1
                              : item->chewCount == 2 ? Item::CHEW_2
                                                     : Item::DEFAULT);

            if (character.calories + caloriesChew >= character.max_capacity() && Item::heldItem)
            {
              character.state_set(Character::SHOCKED);
              dialogueIDs = &dialogue.fullIDs;
              isSpeak = true;
            }
          }
        }
        else if (character.state == Character::ANGRY)
        {
        }
        else
        {

          cursor.play(Cursor::ANIMATION_GRAB);

          if (!isAbleToEat)
          {
            character.state_set(Character::SHOCKED);
            if (caloriesChew > character.max_capacity())
              dialogueIDs = &dialogue.capacityLowIDs;
            else
              dialogueIDs = &dialogue.fullIDs;
          }
          else if (character.is_over_capacity())
            dialogueIDs = &dialogue.feedFullIDs;
          else
          {
            character.state_set(Character::EAGER);
            dialogueIDs = &dialogue.feedHungryIDs;
          }

          if (isHeldItemChanged) isSpeak = true;

          if (isAbleToEat && isByMouth)
            if (character.state != Character::EAT) character.state_set(Character::EAT);

          isRubbing = false;
        }
      }
    }
    else if (isHeldItemChanged && character.state != Character::ANGRY && character.state != Character::STAGE_UP)
      character.state_set(Character::IDLE);

    /* Character */
    if (character.isFinishedFood && character.state == Character::IDLE)
    {
      if (!character.is_over_capacity()) dialogueIDs = &dialogue.eatHungryIDs;

      if (math::random_percent_roll(Character::BURP_BIG_CHANCE))
      {
        character.state_set(Character::BURP_BIG);
        dialogueIDs = &dialogue.burpBigIDs;
      }
      else if (math::random_percent_roll(Character::BURP_SMALL_CHANCE))
      {
        character.state_set(Character::BURP_SMALL);
        dialogueIDs = &dialogue.burpSmallIDs;
      }
      else if (!character.is_over_capacity() && math::random_percent_roll(Character::PAT_CHANCE))
        character.state_set(Character::PAT);

      character.isFinishedFood = false;
      isSpeak = true;
    }

    /* Character */
    if (character.isJustAppeared)
    {
      textWindow.set(dialogue.get("Start"), character);
      isText = true;
      character.isJustAppeared = false;
    }

    if (character.isJustFinalThreshold && !character.isFinalThresholdReached)
    {
      Item::heldItem = nullptr;
      Item::heldItemPrevious = nullptr;
      textWindow.set(dialogue.get("End"), character);
      items.clear();
      character.isFinalThresholdReached = true;
    }

    /* Dialogue */
    if (isSpeak && dialogueIDs) textWindow.set_random(*dialogueIDs, resources, character);

    /* Rubbing/Grabbing */
    bool isHeadRubPossible = math::is_point_in_rectf(character.head_rect_get(), cursor.position) && !Item::heldItem;
    bool isBellyRubPossible = math::is_point_in_rectf(character.belly_rect_get(), cursor.position) && !Item::heldItem;
    bool isTailRubPossible = math::is_point_in_rectf(character.tail_rect_get(), cursor.position) && !Item::heldItem;
    auto isRubPossible = isHeadRubPossible || isBellyRubPossible || isTailRubPossible;

    if (isRubPossible)
    {
      if (!isRubbing) cursor.play(Cursor::ANIMATION_HOVER);

      if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
      {
        isRubbing = true;
        resources.sound_play(audio::RUB);
        if (isHeadRubPossible)
        {
          character.state_set(Character::HEAD_RUB);
          cursor.play(Cursor::ANIMATION_RUB);
        }
        else if (isBellyRubPossible)
        {
          character.state_set(Character::BELLY_RUB);
          cursor.play(Cursor::ANIMATION_GRAB);
        }
        else if (isTailRubPossible)
        {
          character.state_set(Character::TAIL_RUB);
          cursor.play(Cursor::ANIMATION_GRAB);
        }
      }
    }
    else if (Item::hoveredItem)
      cursor.play(Cursor::ANIMATION_HOVER);
    else if (!Item::heldItem)
      cursor.play(Cursor::ANIMATION_DEFAULT);

    if (isRubbing)
    {
      if (isBellyRubPossible && !character.isDigesting)
      {
        auto delta = ImGui::GetIO().MouseDelta;
        auto power = fabs(delta.x) + fabs(delta.y);

        if (character.calories > 0) character.digestionProgress += power * Character::DIGESTION_RUB_BONUS;
      }
      if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || !isRubPossible)
      {
        isRubbing = false;
        character.state_set(Character::IDLE);
      }
    }

    SDL_HideCursor();
    cursor.update();
  }

  void State::render()
  {
    SDL_GL_MakeCurrent(window, context);

    int width{};
    int height{};
    SDL_GetWindowSize(window, &width, &height);

    auto& textureShader = resources.shaders[shader::TEXTURE];
    auto& rectShader = resources.shaders[shader::RECT];

    canvas.bind();

    canvas.clear(glm::vec4(0, 0, 0, 1));

    auto bgModel = math::quad_model_get(vec2(width, height));
    canvas.texture_render(textureShader, resources.textures[texture::BG].id, bgModel);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    character.render(textureShader, rectShader, canvas);

    for (auto& item : items)
      item.render(textureShader, rectShader, canvas);

    cursor.render(textureShader, rectShader, canvas);

    canvas.unbind();

    SDL_GL_SwapWindow(window);
  }

  void State::loop()
  {
    auto currentTick = SDL_GetTicks();
    auto currentUpdate = SDL_GetTicks();

    if (currentUpdate - previousUpdate >= UPDATE_INTERVAL)
    {
      update();
      render();
      previousUpdate = currentUpdate;
    }

    if (currentTick - previousTick >= TICK_INTERVAL)
    {
      tick();
      previousTick = currentTick;
    }

    SDL_Delay(1);
  }
}
