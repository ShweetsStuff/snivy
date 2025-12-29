#include "item.h"
#include "imgui.h"
#include "util/math_.h"

using namespace game::anm2;
using namespace game::util;
using namespace glm;

namespace game
{
  Item* Item::heldItem = nullptr;
  Item* Item::heldItemPrevious = nullptr;
  Item* Item::hoveredItem = nullptr;
  Item* Item::hoveredItemPrevious = nullptr;
  Item* Item::queuedReturnItem = nullptr;

  std::array<Item::Pool, Item::RARITY_COUNT> rarity_pools_get()
  {
    std::array<Item::Pool, Item::RARITY_COUNT> newPools{};
    for (auto& pool : newPools)
      pool.clear();

    for (int i = 0; i < Item::ITEM_COUNT; i++)
    {
      auto& rarity = Item::RARITIES[i];
      newPools[rarity].emplace_back((Item::Type)i);
    }

    return newPools;
  }

  const std::array<Item::Pool, Item::RARITY_COUNT> Item::pools = rarity_pools_get();

  Item::Item(Anm2* _anm2, glm::ivec2 _position, Type _type) : Actor(_anm2, _position, SET, (float)_type)
  {
    this->type = _type;
  }

  void Item::tick() { Actor::tick(); }

  void Item::update(Resources& resources)
  {
    auto bounds = ivec4(position.x - SIZE * 0.5f, position.y - SIZE * 0.5f, SIZE, SIZE);
    auto mousePos = ivec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);

    if (isHeld)
    {
      position = mousePos - ivec2(holdOffset);
      delta = previousPosition - position;

      velocity *= VELOCITY_HOLD_MULTIPLIER;

      if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
      {
        auto power = fabs(delta.x) + fabs(delta.y);

        heldItem = nullptr;

        if (power > THROW_THRESHOLD)
          resources.sound_play(audio::THROW);
        else
          resources.sound_play(audio::RELEASE);

        velocity += delta;
        isHeld = false;
      }
    }
    else if (math::is_point_in_rect(bounds, mousePos))
    {
      hoveredItem = this;

      if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !heldItem)
      {
        heldItem = this;

        resources.sound_play(audio::GRAB);

        isHeld = true;
        holdOffset = mousePos - ivec2(position);
      }
      else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
      {
        queuedReturnItem = this;
      }
    }

    if (!isHeld) velocity.y += GRAVITY;
    position += velocity;

    if (position.x < BOUNDS.x || position.x > BOUNDS.z)
    {
      velocity.x = -velocity.x;
      velocity.x *= FRICTION;

      if (fabs(velocity.x) > BOUNCE_SOUND_THRESHOLD) resources.sound_play(audio::BOUNCE);
    }
    if (position.y < BOUNDS.y || position.y > BOUNDS.w)
    {
      velocity.y = -velocity.y;
      velocity *= FRICTION;

      if (fabs(velocity.y) > BOUNCE_SOUND_THRESHOLD) resources.sound_play(audio::BOUNCE);
    }

    position = glm::clamp(vec2(BOUNDS.x, BOUNDS.y), position, vec2(BOUNDS.z, BOUNDS.w));

    previousPosition = position;
  }

  void Item::state_set(State state)
  {
    this->state = state;

    switch (this->state)
    {
      case DEFAULT:
        play("Default", SET, (float)type);
        break;
      case CHEW_1:
        play("Chew1", SET, (float)type);
        break;
      case CHEW_2:
        play("Chew2", SET, (float)type);
        break;
      default:
        break;
    };
  }
}
