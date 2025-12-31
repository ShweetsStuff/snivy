#pragma once

#include "resource/actor.h"
#include "resources.h"

namespace game
{
  class Item : public resource::Actor
  {
  public:
    static constexpr auto VELOCITY_HOLD_MULTIPLIER = 0.50f;
    static constexpr auto VELOCITY_HOLD_BOOST = 2.5f;
    static constexpr auto BOUNCE_SOUND_THRESHOLD = 2.5f;
    static constexpr auto THROW_MULTIPLIER = 1.0f;
    static constexpr auto THROW_THRESHOLD = 50.0f;
    static constexpr auto FRICTION = 0.75f;
    static constexpr auto GRAVITY = 0.50f;
    static constexpr auto SIZE = 72.0f;
    static constexpr auto CHEW_COUNT_MAX = 2;

    static constexpr auto DEPLOYED_MAX = 10;

    static constexpr auto ANIMATION_STATE = "State";

    static constexpr glm::vec4 BOUNDS = {50, 100, 475, 500};
    static constexpr auto SPAWN_X_MIN = BOUNDS.x;
    static constexpr auto SPAWN_X_MAX = BOUNDS.z + BOUNDS.x;
    static constexpr auto SPAWN_Y_MIN = BOUNDS.y;
    static constexpr auto SPAWN_Y_MAX = BOUNDS.w + BOUNDS.y;

#define CATEGORIES                                                                                                     \
  X(INVALID, "Invalid")                                                                                                \
  X(FOOD, "Food")                                                                                                      \
  X(UTILITY, "Utility")

    enum Category
    {
#define X(symbol, name) symbol,
      CATEGORIES
#undef X
    };

    static constexpr const char* CATEGORY_NAMES[] = {
#define X(symbol, name) name,
        CATEGORIES
#undef X
    };

#undef CATEGORIES

#define RARITIES                                                                                                       \
  X(NO_RARITY, "No Rarity", 0.0f)                                                                                      \
  X(COMMON, "Common", 1.0f)                                                                                            \
  X(UNCOMMON, "Uncommon", 0.75f)                                                                                       \
  X(RARE, "Rare", 0.5f)                                                                                                \
  X(EPIC, "Epic", 0.25f)                                                                                               \
  X(LEGENDARY, "Legendary", 0.125f)                                                                                    \
  X(IMPOSSIBLE, "???", 0.001f)                                                                                         \
  X(SPECIAL, "Special", 0.0f)

    enum Rarity
    {
#define X(symbol, name, chance) symbol,
      RARITIES
#undef X
          RARITY_COUNT
    };

    static constexpr const char* RARITY_NAMES[] = {
#define X(symbol, name, chance) name,
        RARITIES
#undef X
    };

    static constexpr float RARITY_CHANCES[] = {
#define X(symbol, name, chance) chance,
        RARITIES
#undef X
    };

#undef RARITIES

#define FLAVORS                                                                                                        \
  X(FLAVORLESS, "None")                                                                                                \
  X(SWEET, "Sweet")                                                                                                    \
  X(BITTER, "Bitter")                                                                                                  \
  X(SPICY, "Spicy")                                                                                                    \
  X(MINT, "Mint")                                                                                                      \
  X(CITRUS, "Citrus")                                                                                                  \
  X(MOCHA, "Mocha")                                                                                                    \
  X(SPICE, "Spice")

    enum Flavor
    {
#define X(symbol, name) symbol,
      FLAVORS
#undef X
          FLAVOR_COUNT
    };

    static constexpr const char* FLAVOR_NAMES[] = {
#define X(symbol, name) name,
        FLAVORS
#undef X
    };

#undef FLAVORS

    // clang-format off
#define ITEMS                                                                                                                \
  X(NONE, INVALID, FLAVORLESS, NO_RARITY, "", "", 0, 0, 0)                                                                      \
  X(POKE_PUFF_BASIC_SWEET, FOOD, SWEET, COMMON, "Poké Puff (Basic, Sweet)", "A basic sweet Poké Puff.", 100.0f, 0, 0)        \
  X(POKE_PUFF_BASIC_MINT, FOOD, MINT, COMMON, "Poké Puff (Basic, Mint)", "A basic minty Poké Puff.", 100.0f, 0, 0)           \
  X(POKE_PUFF_BASIC_CITRUS, FOOD, CITRUS, COMMON, "Poké Puff (Basic, Citrus)", "A basic citrusy Poké Puff.", 100.0f, 0, 0)   \
  X(POKE_PUFF_BASIC_MOCHA, FOOD, MOCHA, COMMON, "Poké Puff (Basic, Mocha)", "A basic mocha Poké Puff.", 100.0f, 0, 0)        \
  X(POKE_PUFF_BASIC_SPICE, FOOD, SPICE, COMMON, "Poké Puff (Basic, Spice)", "A basic spice Poké Puff.", 100.0f, 0, 0) \
  X(POKE_PUFF_FROSTED_SWEET, FOOD, SWEET, UNCOMMON, "Poké Puff (Frosted, Sweet)", "A frosted sweet Poké Puff.", 250.0f, 0, 0) \
  X(POKE_PUFF_FROSTED_MINT, FOOD, MINT, UNCOMMON, "Poké Puff (Frosted, Mint)", "A frosted minty Poké Puff.", 250.0f, 0, 0) \
  X(POKE_PUFF_FROSTED_CITRUS, FOOD, MINT, UNCOMMON, "Poké Puff (Frosted, Citrus)", "A frosted citrusy Poké Puff.", 250.0f, 0, 0) \
  X(POKE_PUFF_FROSTED_MOCHA, FOOD, MOCHA, UNCOMMON, "Poké Puff (Frosted, Mocha)", "A frosted mocha Poké Puff.", 250.0f, 0, 0) \
  X(POKE_PUFF_FROSTED_SPICE, FOOD, SPICE, UNCOMMON, "Poké Puff (Frosted, Spice)", "A frosted spice Poké Puff.", 250.0f, 0, 0) \
  X(POKE_PUFF_FANCY_SWEET, FOOD, SWEET, RARE, "Poké Puff (Fancy, Sweet)", "A fancy sweet Poké Puff, adorned with a cherry.", 500.0f, 0, 0) \
  X(POKE_PUFF_FANCY_MINT, FOOD, MINT, RARE, "Poké Puff (Fancy, Mint)", "A fancy minty Poké Puff, adorned with a crescent moon-shaped candy.", 500.0f, 0, 0) \
  X(POKE_PUFF_FANCY_CITRUS, FOOD, CITRUS, RARE, "Poké Puff (Fancy, Citrus)", "A fancy citrus Poké Puff, adorned with an orange slice.", 500.0f, 0, 0) \
  X(POKE_PUFF_FANCY_MOCHA, FOOD, MOCHA, RARE, "Poké Puff (Fancy, Mocha)", "A fancy mocha Poké Puff, adorned with a morsel of white chocolate.", 500.0f, 0, 0) \
  X(POKE_PUFF_FANCY_SPICE, FOOD, SPICE, RARE, "Poké Puff (Fancy, Spice)", "A fancy spice Poké Puff, adorned with a morsel of dark chocolate.", 500.0f, 0, 0) \
  X(POKE_PUFF_DELUXE_SWEET, FOOD, SWEET, EPIC, "Poké Puff (Deluxe, Sweet)", "A deluxe sweet Poké Puff; frosted and adorned with a cherry.", 1000.0f, 0, 0) \
  X(POKE_PUFF_DELUXE_MINT, FOOD, MINT, EPIC, "Poké Puff (Deluxe, Mint)", "A deluxe minty Poké Puff; frosted and adorned with a crescent moon-shapedd candy.", 1000.0f, 0, 0) \
  X(POKE_PUFF_DELUXE_CITRUS, FOOD, CITRUS, EPIC, "Poké Puff (Deluxe, Citrus)", "A deluxe citrusy Poké Puff; frosted and adorned with an orange slice.", 1000.0f, 0, 0) \
  X(POKE_PUFF_DELUXE_MOCHA, FOOD, MOCHA, EPIC, "Poké Puff (Deluxe, Mocha)", "A deluxe mocha Poké Puff; frosted and adorned with a morsel of white chocolate.", 1000.0f, 0, 0) \
  X(POKE_PUFF_DELUXE_SPICE, FOOD, SPICE, EPIC, "Poké Puff (Deluxe, Spice)", "A deluxe spice Poké Puff; frosted and adorned with a morsel of dark chocolate.", 1000.0f, 0, 0) \
  X(POKE_PUFF_SUPREME_SPRING, FOOD, SWEET, LEGENDARY, "Poké Puff (Supreme Spring)", "A supreme Poké Puff that tastes like the sweet cherry blossoms of spring.", 2500.0f, 0, 0) \
  X(POKE_PUFF_SUPREME_SUMMER, FOOD, CITRUS, LEGENDARY, "Poké Puff (Supreme Summer)", "A supreme Poké Puff that tastes like a tropical summer vacation.", 2500.0f, 0, 0) \
  X(POKE_PUFF_SUPREME_AUTUMN, FOOD, CITRUS, LEGENDARY, "Poké Puff (Supreme Autumn)", "A supreme Poké Puff that tastes like a bountiful autumn harvest.", 2500.0f, 0, 0) \
  X(POKE_PUFF_SUPREME_WINTER, FOOD, SPICE, LEGENDARY, "Poké Puff (Supreme Winter)", "A supreme Poké Puff that tastes like a frosty winter wonderland.", 2500.0f, 0, 0) \
  X(POKE_PUFF_SUPREME_WISH, FOOD, SPICE, IMPOSSIBLE, "Poké Puff (Supreme Wish)", "A supreme Poké Puff that tastes like a cherished birthday celebration.\nIt also is the biggest calorie nuke ever.\nCan Snivy eat it!?\nAlso, statistically, it might be your birthday today, right?", 10000.0f, 0, 0) \
  X(POKE_PUFF_SUPREME_HONOR, FOOD, MOCHA, SPECIAL, "Poké Puff (Supreme Honor)", "A supreme Poké Puff that tastes like a monumental victory.\nAwarded for achieving a superb Play score.\nYou've earned it...if Snivy can eat it!", 2500.0f, 0.025f, 1.0f) \
  X(BERRY_CHERI, FOOD, SPICY, UNCOMMON, "Cheri Berry", "A spherical red berry that cures paralysis\n...oh, and it also excites one's stomach.", 25.0f, 0.000833f, 0) \
  X(BERRY_TAMATO, FOOD, SPICY, RARE, "Tamato Berry", "A spiny crimson berry that lowers speed and raises friendship.\n...oh, and it also greatly excites one's stomach.", 50.0f, 0.0025f, 0) \
  X(BERRY_TOUGA, FOOD, SPICY, EPIC, "Touga Berry", "An obscure, searing-red berry from faraway lands.\nIt cures confusion...and it'll make one's stomach a burning inferno.", 100.0f, 0.00833f, 0) \
  X(BERRY_PECHA, FOOD, SWEET, UNCOMMON, "Pecha Berry", "A plump and juicy pink berry that cures poison.\n...oh, and its sugars will stir appetite.", 25.0f, 0, 0.05f) \
  X(BERRY_MAGOST, FOOD, SWEET, RARE, "Magost Berry", "A spherical, shining berry that packs a punch with a fibrious inside.\n...oh, and its sugars will greatly stir appetite.", 50.0f, 0, 0.15f) \
  X(BERRY_WATMEL, FOOD, SWEET, EPIC, "Watmel Berry", "A truly bountiful berry with a brilliant green and pink pattern.\nWhispering tales say that if one eats such a berry,\ntheir appetite will increase to a point that they will soon bare the berry's shape.", 100.0f, 0, 0.50f) \
  X(BERRY_AGUAV, FOOD, BITTER, RARE, "Aguav Berry", "A bitter berry that will restore health for hurt eaters who enjoy the flavor.\n...oh, also its taste will soothe an upset stomach.\n(Only use this if you really want to decrease Snivy's digestion.)", 50.0f, -0.01666f, 0) \
  X(BERRY_POMEG, FOOD, FLAVORLESS, IMPOSSIBLE, "Pomeg Berry", "cG9tZWcgYmVycnkgcG9tZWcgYmVycnkgcG9tZWcgYmVycnk=", -2500.0f, 0.2, 2.0f)
    // clang-format on

    enum Type
    {
#define X(symbol, category, flavor, rarity, name, description, calories, digestionRateBonus, eatSpeedBonus) symbol,
      ITEMS
#undef X
          ITEM_COUNT
    };

    static constexpr Category CATEGORIES[] = {
#define X(symbol, category, flavor, rarity, name, description, calories, digestionRateBonus, eatSpeedBonus) category,
        ITEMS
#undef X
    };

    static constexpr Flavor FLAVORS[] = {
#define X(symbol, category, flavor, rarity, name, description, calories, digestionRateBonus, eatSpeedBonus) flavor,
        ITEMS
#undef X
    };

    static constexpr Rarity RARITIES[] = {
#define X(symbol, category, flavor, rarity, name, description, calories, digestionRateBonus, eatSpeedBonus) rarity,
        ITEMS
#undef X
    };

    static constexpr const char* NAMES[] = {
#define X(symbol, category, flavor, rarity, name, description, calories, digestionRateBonus, eatSpeedBonus) name,
        ITEMS
#undef X
    };

    static constexpr const char* DESCRIPTIONS[] = {
#define X(symbol, category, flavor, rarity, name, description, calories, digestionRateBonus, eatSpeedBonus) description,
        ITEMS
#undef X
    };

    static constexpr float CALORIES[] = {
#define X(symbol, category, flavor, rarity, name, description, calories, digestionRateBonus, eatSpeedBonus) calories,
        ITEMS
#undef X
    };

    static constexpr float DIGESTION_RATE_BONUSES[] = {
#define X(symbol, category, flavor, rarity, name, description, calories, digestionRateBonus, eatSpeedBonus)            \
  digestionRateBonus,
        ITEMS
#undef X
    };

    static constexpr float EAT_SPEED_BONUSES[] = {
#define X(symbol, category, flavor, rarity, name, description, calories, digestionRateBonus, eatSpeedBonus)            \
  eatSpeedBonus,
        ITEMS
#undef X
    };

#undef ITEMS

    enum State
    {
      DEFAULT,
      CHEW_1,
      CHEW_2
    };

    static Item* heldItem;
    static Item* heldItemPrevious;
    static Item* hoveredItem;
    static Item* hoveredItemPrevious;
    static Item* queuedReturnItem;

    using Pool = std::vector<Type>;
    static const std::array<Pool, RARITY_COUNT> pools;

    Type type{NONE};
    State state{DEFAULT};
    int chewCount{0};
    bool isToBeDeleted{};

    glm::vec2 delta{};
    glm::vec2 previousPosition{};
    glm::vec2 velocity{};
    glm::vec2 holdOffset{};

    bool isHeld{};

    Item(anm2::Anm2*, glm::ivec2, Type);
    void state_set(State);
    void tick();
    void update(Resources& resources, Camera& camera);
  };
}
