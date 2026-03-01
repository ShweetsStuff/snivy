#pragma once

#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h>
#else
  #include <glad/glad.h>
#endif

#include <cstdint>
#include <filesystem>
#include <glm/ext/vector_int2.hpp>
#include <memory>

#include "../util/physfs.hpp"

namespace game::resource
{
  class Texture
  {
  public:
    static constexpr auto CHANNELS = 4;

    GLuint id{};
    glm::ivec2 size{};
    int channels{};

    bool is_valid() const;

    Texture() = default;
    ~Texture();
    Texture(const Texture&);
    Texture(Texture&&) noexcept;
    Texture& operator=(const Texture&);
    Texture& operator=(Texture&&) noexcept;
    Texture(const std::filesystem::path&);
    Texture(const util::physfs::Path&);
    void init(const uint8_t*);

  private:
    std::shared_ptr<GLuint> idShared{};
  };
}
