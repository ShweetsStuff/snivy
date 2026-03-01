#include "shader.hpp"

#include "../log.hpp"

namespace game::resource
{
  Shader::Shader(const char* vertex, const char* fragment)
  {
    id = glCreateProgram();

    auto compile = [&](const GLuint& shaderHandle, const char* text, const char* stage)
    {
      int isCompile{};
      glShaderSource(shaderHandle, 1, &text, nullptr);
      glCompileShader(shaderHandle);
      glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &isCompile);
      if (!isCompile)
      {
        GLint logLength = 0;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(logLength, '\0');
        if (logLength > 0) glGetShaderInfoLog(shaderHandle, logLength, nullptr, log.data());
        logger.error(std::format("Failed to compile {} shader: {}", stage, log));
        return false;
      }
      return true;
    };

    auto vertexHandle = glCreateShader(GL_VERTEX_SHADER);
    auto fragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);

    if (!(compile(vertexHandle, vertex, "vertex") && compile(fragmentHandle, fragment, "fragment")))
    {
      glDeleteShader(vertexHandle);
      glDeleteShader(fragmentHandle);
      glDeleteProgram(id);
      id = 0;
      return;
    }

    glAttachShader(id, vertexHandle);
    glAttachShader(id, fragmentHandle);

    glLinkProgram(id);

    auto isLinked = GL_FALSE;
    glGetProgramiv(id, GL_LINK_STATUS, &isLinked);

    if (!isLinked)
    {
      glDeleteProgram(id);
      logger.error(std::format("Failed to link shader: {}", id));
      id = 0;
    }
    else
      logger.info(std::format("Initialized shader: {}", id));

    glDeleteShader(vertexHandle);
    glDeleteShader(fragmentHandle);
  }

  Shader::~Shader()
  {
    if (is_valid()) glDeleteProgram(id);
  }

  bool Shader::is_valid() const { return id != 0; }

  Shader& Shader::operator=(Shader&& other) noexcept
  {
    if (this != &other)
    {
      if (is_valid()) glDeleteProgram(id);
      id = other.id;
      other.id = 0;
    }
    return *this;
  }
}
