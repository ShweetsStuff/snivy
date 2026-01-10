#pragma once

#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h>
#else
  #include <glad/glad.h>
#endif

#include "resource/shader.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace game
{
  class Canvas
  {
    static constexpr float TEXTURE_VERTICES[] = {0, 0, 0.0f, 0.0f, 1, 0, 1.0f, 0.0f,
                                                 1, 1, 1.0f, 1.0f, 0, 1, 0.0f, 1.0f};

    static constexpr GLuint TEXTURE_INDICES[] = {0, 1, 2, 2, 3, 0};

    static constexpr float RECT_VERTICES[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

    static GLuint textureVAO;
    static GLuint textureVBO;
    static GLuint textureEBO;
    static GLuint rectVAO;
    static GLuint rectVBO;
    static bool isStaticInit;

  public:
    GLuint fbo{};
    GLuint rbo{};
    GLuint texture{};

    glm::vec2 size{};

    bool isDefault{};

    Canvas() = default;
    Canvas(glm::vec2, bool isDefault = false);
    ~Canvas();
    glm::mat4 transform_get() const;
    glm::mat4 view_get() const;
    glm::mat4 projection_get() const;
    void texture_render(resource::Shader&, GLuint, glm::mat4&, glm::vec4 = glm::vec4(1.0f), glm::vec3 = {},
                        float* = (float*)TEXTURE_VERTICES) const;
    void rect_render(resource::Shader&, glm::mat4&, glm::vec4 = glm::vec4(0, 0, 1, 1)) const;
    void render(resource::Shader&, glm::mat4&, glm::vec4 = glm::vec4(1.0f), glm::vec3 = {}) const;
    void bind() const;
    void unbind() const;
    void clear(glm::vec4 color = glm::vec4(0, 0, 0, 1)) const;
    bool is_valid() const;
  };
}
