#include "texture.h"

#if defined(__clang__) || defined(__GNUC__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
  #pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#if defined(__clang__) || defined(__GNUC__)
  #pragma GCC diagnostic pop
#endif

#include <iostream>

using namespace glm;

namespace game::resource
{
  bool Texture::is_valid() const { return id != 0; }

  void Texture::retain()
  {
    if (refCount) ++(*refCount);
  }

  void Texture::release()
  {
    if (refCount)
    {
      if (--(*refCount) == 0)
      {
        if (is_valid()) glDeleteTextures(1, &id);
        delete refCount;
      }
      refCount = nullptr;
    }
    else if (is_valid())
    {
      glDeleteTextures(1, &id);
    }

    id = 0;
  }

  Texture::~Texture() { release(); }

  Texture::Texture(const Texture& other)
  {
    id = other.id;
    size = other.size;
    channels = other.channels;
    refCount = other.refCount;
    retain();
  }

  Texture::Texture(Texture&& other) noexcept
  {
    id = other.id;
    size = other.size;
    channels = other.channels;
    refCount = other.refCount;

    other.id = 0;
    other.size = {};
    other.channels = 0;
    other.refCount = nullptr;
  }

  Texture& Texture::operator=(const Texture& other)
  {
    if (this != &other)
    {
      release();
      id = other.id;
      size = other.size;
      channels = other.channels;
      refCount = other.refCount;
      retain();
    }
    return *this;
  }

  Texture& Texture::operator=(Texture&& other) noexcept
  {
    if (this != &other)
    {
      release();
      id = other.id;
      size = other.size;
      channels = other.channels;
      refCount = other.refCount;

      other.id = 0;
      other.size = {};
      other.channels = 0;
      other.refCount = nullptr;
    }
    return *this;
  }

  Texture::Texture(const std::filesystem::path& path)
  {
    if (auto data = stbi_load(path.c_str(), &size.x, &size.y, nullptr, CHANNELS); data)
    {
      glGenTextures(1, &id);
      glBindTexture(GL_TEXTURE_2D, id);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glBindTexture(GL_TEXTURE_2D, 0);
      stbi_image_free(data);
      channels = CHANNELS;
      refCount = new int(1);
      std::cout << "Initialized texture: '" << path.string() << "\n";
    }
    else
    {
      id = 0;
      size = {};
      channels = 0;
      refCount = nullptr;
      std::cout << "Failed to initialize texture: '" << path.string() << "'\n";
    }
  }
}
