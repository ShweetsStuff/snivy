#include "texture.hpp"

#include <SDL3/SDL_surface.h>
#include <string>
#include <unordered_map>

#include "../log.hpp"

using namespace glm;
using namespace game::util;

namespace game::resource
{
  struct CachedTexture
  {
    std::weak_ptr<GLuint> idShared{};
    glm::ivec2 size{};
    int channels{};
  };

  static std::unordered_map<std::string, CachedTexture> textureCache{};

  static bool cache_get(const std::string& key, std::shared_ptr<GLuint>& idShared, GLuint& id, ivec2& size, int& channels)
  {
    auto it = textureCache.find(key);
    if (it == textureCache.end()) return false;

    auto shared = it->second.idShared.lock();
    if (!shared)
    {
      textureCache.erase(it);
      return false;
    }

    idShared = shared;
    id = *shared;
    size = it->second.size;
    channels = it->second.channels;
    return true;
  }

  static void cache_set(const std::string& key, const std::shared_ptr<GLuint>& idShared, ivec2 size, int channels)
  {
    if (!idShared) return;
    textureCache[key] = CachedTexture{.idShared = idShared, .size = size, .channels = channels};
  }

  static std::shared_ptr<GLuint> texture_id_make(GLuint id)
  {
    return std::shared_ptr<GLuint>(new GLuint(id),
                                   [](GLuint* p)
                                   {
                                     if (!p) return;
                                     if (*p != 0) glDeleteTextures(1, p);
                                     delete p;
                                   });
  }

  bool Texture::is_valid() const { return id != 0; }

  Texture::~Texture()
  {
    idShared.reset();
    id = 0;
  }

  Texture::Texture(const Texture& other)
  {
    id = other.id;
    size = other.size;
    channels = other.channels;
    idShared = other.idShared;
  }

  Texture::Texture(Texture&& other) noexcept
  {
    id = other.id;
    size = other.size;
    channels = other.channels;
    idShared = std::move(other.idShared);

    other.id = 0;
    other.size = {};
    other.channels = 0;
  }

  Texture& Texture::operator=(const Texture& other)
  {
    if (this != &other)
    {
      idShared = other.idShared;
      id = other.id;
      size = other.size;
      channels = other.channels;
    }
    return *this;
  }

  Texture& Texture::operator=(Texture&& other) noexcept
  {
    if (this != &other)
    {
      idShared.reset();
      id = other.id;
      size = other.size;
      channels = other.channels;
      idShared = std::move(other.idShared);

      other.id = 0;
      other.size = {};
      other.channels = 0;
    }
    return *this;
  }

  void Texture::init(const uint8_t* data)
  {
    idShared.reset();
    id = 0;

    GLuint newId{};
    glGenTextures(1, &newId);
    glBindTexture(GL_TEXTURE_2D, newId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    channels = CHANNELS;

    idShared = texture_id_make(newId);
    id = newId;
  }

  Texture::Texture(const std::filesystem::path& path)
  {
    auto key = std::string("fs:") + path.string();
    if (cache_get(key, idShared, id, size, channels))
    {
      logger.info(std::format("Using cached texture: {}", path.string()));
      return;
    }

    if (auto surface = SDL_LoadPNG(path.c_str()))
    {
      auto rgbaSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
      SDL_DestroySurface(surface);
      surface = rgbaSurface;
      this->size = ivec2(surface->w, surface->h);
      init((const uint8_t*)surface->pixels);
      SDL_DestroySurface(surface);
      cache_set(key, idShared, this->size, channels);
      logger.info(std::format("Initialized texture: {}", path.string()));
    }
    else
      logger.error(std::format("Failed to initialize texture: {} ({})", path.string(), SDL_GetError()));
  }

  Texture::Texture(const physfs::Path& path)
  {
    if (!path.is_valid())
    {
      logger.error(
          std::format("Failed to initialize texture from PhysicsFS path: {}", path.c_str(), physfs::error_get()));
      return;
    }

    auto key = std::string("physfs:") + path.c_str();
    if (cache_get(key, idShared, id, size, channels))
    {
      logger.info(std::format("Using cached texture: {}", path.c_str()));
      return;
    }

    auto buffer = path.read();

    if (buffer.empty())
    {
      logger.error(
          std::format("Failed to initialize texture from PhysicsFS path: {} ({})", path.c_str(), physfs::error_get()));
      return;
    }

    auto ioStream = SDL_IOFromConstMem(buffer.data(), buffer.size());

    if (auto surface = SDL_LoadPNG_IO(ioStream, true))
    {
      auto rgbaSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
      SDL_DestroySurface(surface);
      surface = rgbaSurface;
      this->size = ivec2(surface->w, surface->h);
      init((const uint8_t*)surface->pixels);
      SDL_DestroySurface(surface);
      cache_set(key, idShared, this->size, channels);
      logger.info(std::format("Initialized texture: {}", path.c_str()));
    }
    else
      logger.error(std::format("Failed to initialize texture: {} ({})", path.c_str(), SDL_GetError()));
  }
}
