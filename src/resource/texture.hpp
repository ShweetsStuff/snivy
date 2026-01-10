#pragma once

#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h>
#else
  #include <glad/glad.h>
#endif

#include <filesystem>
#include <glm/ext/vector_int2.hpp>

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

  private:
    int* refCount{nullptr};
    void retain();
    void release();
  };
}
