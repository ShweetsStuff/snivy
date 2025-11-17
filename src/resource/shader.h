#pragma once

#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h>
#else
  #include <glad/glad.h>
#endif

namespace game::resource::shader
{
  struct Info
  {
    const char* vertex{};
    const char* fragment{};
  };

#ifdef __EMSCRIPTEN__
  constexpr auto VERTEX = R"(#version 300 es
  layout (location = 0) in vec2 i_position;
  layout (location = 1) in vec2 i_uv;
  out vec2 v_uv;
  uniform mat4 u_model;
  uniform mat4 u_view;
  uniform mat4 u_projection;
  void main()
  {
      v_uv = i_uv;
      mat4 transform = u_projection * u_view * u_model;
      gl_Position = transform * vec4(i_position, 0.0, 1.0);
  }
  )";

  constexpr auto FRAGMENT = R"(#version 300 es
  precision mediump float;
  in vec2 v_uv;
  uniform sampler2D u_texture;
  uniform vec4 u_tint;
  uniform vec3 u_color_offset;
  out vec4 o_fragColor;
  void main()
  {
      vec4 texColor = texture(u_texture, v_uv);
      texColor *= u_tint;
      texColor.rgb += u_color_offset;
      o_fragColor = texColor;
  }
  )";
#else
  constexpr auto VERTEX = R"(#version 330 core
  layout (location = 0) in vec2 i_position;
  layout (location = 1) in vec2 i_uv;
  out vec2 v_uv;
  uniform mat4 u_model;
  uniform mat4 u_view;
  uniform mat4 u_projection;
  void main()
  {
      v_uv = i_uv;
      mat4 transform = u_projection * u_view * u_model;
      gl_Position = transform * vec4(i_position, 0.0, 1.0);
  }
  )";

  constexpr auto FRAGMENT = R"(#version 330 core
  in vec2 v_uv;
  uniform sampler2D u_texture;
  uniform vec4 u_tint;
  uniform vec3 u_color_offset;
  out vec4 o_fragColor;
  void main()
  {
      vec4 texColor = texture(u_texture, v_uv);
      texColor *= u_tint;
      texColor.rgb += u_color_offset;
      o_fragColor = texColor;
  }
  )";
#endif

  constexpr auto UNIFORM_MODEL = "u_model";
  constexpr auto UNIFORM_VIEW = "u_view";
  constexpr auto UNIFORM_PROJECTION = "u_projection";
  constexpr auto UNIFORM_TEXTURE = "u_texture";
  constexpr auto UNIFORM_TINT = "u_tint";
  constexpr auto UNIFORM_COLOR_OFFSET = "u_color_offset";

#define SHADERS X(TEXTURE, VERTEX, FRAGMENT)

  enum Type
  {
#define X(symbol, vertex, fragment) symbol,
    SHADERS
#undef X
        COUNT
  };

  constexpr Info INFO[] = {
#define X(symbol, vertex, fragment) {vertex, fragment},
      SHADERS
#undef X
  };
}

namespace game::resource
{
  class Shader
  {
  public:
    GLint id{};

    Shader() = default;
    Shader(const char*, const char*);
    bool is_valid() const;
    ~Shader();

    Shader& operator=(Shader&&) noexcept;
  };
}
