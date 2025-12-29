#include "canvas.h"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace game::resource;

namespace game
{
  GLuint Canvas::textureVAO = 0;
  GLuint Canvas::textureVBO = 0;
  GLuint Canvas::textureEBO = 0;
  GLuint Canvas::rectVAO = 0;
  GLuint Canvas::rectVBO = 0;
  bool Canvas::isStaticInit = false;

  Canvas::Canvas(vec2 size, bool isDefault)
  {
    this->size = size;

    if (isDefault)
    {
      fbo = 0;
      rbo = 0;
      texture = 0;
      this->isDefault = true;
    }
    else
    {
      glGenFramebuffers(1, &fbo);
      glGenRenderbuffers(1, &rbo);
      glGenTextures(1, &texture);

      glBindFramebuffer(GL_FRAMEBUFFER, fbo);

      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

      glBindRenderbuffer(GL_RENDERBUFFER, rbo);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    if (!isStaticInit)
    {
      glGenVertexArrays(1, &textureVAO);
      glGenBuffers(1, &textureVBO);
      glGenBuffers(1, &textureEBO);

      glBindVertexArray(textureVAO);

      glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, nullptr, GL_DYNAMIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textureEBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TEXTURE_INDICES), TEXTURE_INDICES, GL_DYNAMIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

      glBindVertexArray(0);

      // Rect
      glGenVertexArrays(1, &rectVAO);
      glGenBuffers(1, &rectVBO);

      glBindVertexArray(rectVAO);

      glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(RECT_VERTICES), RECT_VERTICES, GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

      glBindVertexArray(0);
    }
  }

  Canvas::~Canvas()
  {
    if (!isDefault)
    {
      if (fbo) glDeleteFramebuffers(1, &fbo);
      if (rbo) glDeleteRenderbuffers(1, &rbo);
      if (texture) glDeleteTextures(1, &texture);
    }
  }

  mat4 Canvas::view_get() const { return mat4{1.0f}; }
  mat4 Canvas::projection_get() const
  {
    if (isDefault) return glm::ortho(0.0f, (float)size.x, (float)size.y, 0.0f, -1.0f, 1.0f);
    return glm::ortho(0.0f, (float)size.x, 0.0f, (float)size.y, -1.0f, 1.0f);
  }

  void Canvas::texture_render(Shader& shader, GLuint textureId, mat4& model, vec4 tint, vec3 colorOffset,
                              float* vertices) const
  {
    glUseProgram(shader.id);

    glUniform1i(glGetUniformLocation(shader.id, shader::UNIFORM_TEXTURE), 0);
    glUniform3fv(glGetUniformLocation(shader.id, shader::UNIFORM_COLOR_OFFSET), 1, value_ptr(colorOffset));
    glUniform4fv(glGetUniformLocation(shader.id, shader::UNIFORM_TINT), 1, value_ptr(tint));

    glUniformMatrix4fv(glGetUniformLocation(shader.id, shader::UNIFORM_MODEL), 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.id, shader::UNIFORM_VIEW), 1, GL_FALSE, value_ptr(view_get()));
    glUniformMatrix4fv(glGetUniformLocation(shader.id, shader::UNIFORM_PROJECTION), 1, GL_FALSE,
                       value_ptr(projection_get()));

    glBindVertexArray(textureVAO);

    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TEXTURE_VERTICES), vertices, GL_DYNAMIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
  }

  void Canvas::rect_render(Shader& shader, mat4& model, vec4 color) const
  {
    glUseProgram(shader.id);

    glUniform4fv(glGetUniformLocation(shader.id, shader::UNIFORM_COLOR), 1, value_ptr(color));

    glUniformMatrix4fv(glGetUniformLocation(shader.id, shader::UNIFORM_MODEL), 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.id, shader::UNIFORM_VIEW), 1, GL_FALSE, value_ptr(view_get()));
    glUniformMatrix4fv(glGetUniformLocation(shader.id, shader::UNIFORM_PROJECTION), 1, GL_FALSE,
                       value_ptr(projection_get()));

    glBindVertexArray(rectVAO);

    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glBindVertexArray(0);
    glUseProgram(0);
  }

  void Canvas::render(Shader& shader, mat4& model, vec4 tint, vec3 colorOffset) const
  {
    texture_render(shader, texture, model, tint, colorOffset);
  }

  void Canvas::bind() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glViewport(0, 0, size.x, size.y);
  }

  void Canvas::clear(vec4 color) const
  {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void Canvas::unbind() const
  {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  bool Canvas::is_valid() const { return fbo != 0 || isDefault; };
}
