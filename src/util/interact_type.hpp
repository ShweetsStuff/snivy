#pragma once

namespace game
{
#define LIST                                                                                                           \
  X(RUB, "Rub")                                                                                                        \
  X(KISS, "Kiss")                                                                                                      \
  X(SMACK, "Smack")

  enum InteractType
  {
#define X(symbol, string) symbol,
    LIST
#undef X
  };

  static constexpr const char* INTERACT_TYPE_STRINGS[] = {
#define X(symbol, string) string,
      LIST
#undef X
  };

#undef LIST
}