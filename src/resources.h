#pragma once

#include "resource/anm2.h"
#include "resource/audio.h"
#include "resource/dialogue.h"
#include "resource/font.h"
#include "resource/shader.h"

namespace game::audio
{
#define AUDIO                                                                                                          \
  X(BLIP, "resources/sfx/blip.ogg")                                                                                    \
  X(ADVANCE, "resources/sfx/advance.ogg")                                                                              \
  X(BOUNCE, "resources/sfx/bounce.ogg")                                                                                \
  X(BURP_1, "resources/sfx/burp1.ogg")                                                                                 \
  X(BURP_2, "resources/sfx/burp2.ogg")                                                                                 \
  X(BURP_3, "resources/sfx/burp3.ogg")                                                                                 \
  X(GRAB, "resources/sfx/grab.ogg")                                                                                    \
  X(MISS, "resources/sfx/miss.ogg")                                                                                    \
  X(OK, "resources/sfx/ok.ogg")                                                                                        \
  X(GOOD, "resources/sfx/good.ogg")                                                                                    \
  X(GREAT, "resources/sfx/great.ogg")                                                                                  \
  X(EXCELLENT, "resources/sfx/excellent.ogg")                                                                          \
  X(PERFECT, "resources/sfx/perfect.ogg")                                                                              \
  X(FALL, "resources/sfx/fall.ogg")                                                                                    \
  X(COMMON, "resources/sfx/common.ogg")                                                                                \
  X(UNCOMMON, "resources/sfx/uncommon.ogg")                                                                            \
  X(RARE, "resources/sfx/rare.ogg")                                                                                    \
  X(EPIC, "resources/sfx/epic.ogg")                                                                                    \
  X(LEGENDARY, "resources/sfx/legendary.ogg")                                                                          \
  X(SPECIAL, "resources/sfx/special.ogg")                                                                              \
  X(IMPOSSIBLE, "resources/sfx/impossible.ogg")                                                                        \
  X(SCORE_LOSS, "resources/sfx/scoreLoss.ogg")                                                                         \
  X(HIGH_SCORE, "resources/sfx/highScore.ogg")                                                                         \
  X(HIGH_SCORE_BIG, "resources/sfx/highScoreBig.ogg")                                                                  \
  X(HIGH_SCORE_LOSS, "resources/sfx/highScoreLoss.ogg")                                                                \
  X(GURGLE_1, "resources/sfx/gurgle1.ogg")                                                                             \
  X(GURGLE_2, "resources/sfx/gurgle2.ogg")                                                                             \
  X(GURGLE_3, "resources/sfx/gurgle3.ogg")                                                                             \
  X(RELEASE, "resources/sfx/release.ogg")                                                                              \
  X(SUMMON, "resources/sfx/summon.ogg")                                                                                \
  X(RETURN, "resources/sfx/return.ogg")                                                                                \
  X(DISPOSE, "resources/sfx/dispose.ogg")                                                                              \
  X(RUB, "resources/sfx/rub.ogg")                                                                                      \
  X(THROW, "resources/sfx/throw.ogg")

  enum Type
  {
#define X(symbol, path) symbol,
    AUDIO
#undef X
        COUNT
  };

  static constexpr const char* PATHS[] = {
#define X(symbol, path) path,
      AUDIO
#undef X
  };

#undef AUDIO

  static constexpr Type BURPS[] = {BURP_1, BURP_2, BURP_3};
  static constexpr Type GURGLES[] = {GURGLE_1, GURGLE_2, GURGLE_3};
}

namespace game::texture
{
#define TEXTURES X(BG, "resources/gfx/bg.png")

  enum Type
  {
#define X(symbol, path) symbol,
    TEXTURES
#undef X
        COUNT
  };

  static constexpr const char* PATHS[] = {
#define X(symbol, path) path,
      TEXTURES
#undef X
  };

#undef TEXTURES
}

namespace game::anm2
{
#define ANM2                                                                                                           \
  X(CHARACTER, "resources/anm2/snivy.anm2")                                                                            \
  X(ITEMS, "resources/anm2/items.anm2")                                                                                \
  X(CURSOR, "resources/anm2/cursor.anm2")

  enum Anm2Type
  {
#define X(symbol, path) symbol,
    ANM2
#undef X
        COUNT
  };

  static constexpr const char* PATHS[] = {
#define X(symbol, path) path,
      ANM2
#undef X
  };

#undef ANM2
}

namespace game
{
  class Resources
  {

  public:
    resource::Shader shaders[resource::shader::COUNT];
    resource::Audio audio[audio::COUNT];
    resource::Texture textures[texture::COUNT];
    anm2::Anm2 anm2s[anm2::COUNT];
    resource::Font font{"resources/font/font.ttf"};
    resource::Dialogue dialogue{"resources/dialogue.xml"};

    Resources();
    void sound_play(audio::Type);
    void set_audio_gain(float vol);
  };
}