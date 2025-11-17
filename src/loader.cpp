#include "loader.h"

#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h>
#else
  #include <glad/glad.h>
#endif

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>
#include <iostream>

#include <SDL3_mixer/SDL_mixer.h>

#ifdef __EMSCRIPTEN__
constexpr auto GL_VERSION_MAJOR = 3;
constexpr auto GL_VERSION_MINOR = 0;
constexpr auto GLSL_VERSION = "#version 300 es";
#else
constexpr auto GL_VERSION_MAJOR = 3;
constexpr auto GL_VERSION_MINOR = 3;
constexpr auto GLSL_VERSION = "#version 330";
#endif

constexpr auto WINDOW_ROUNDING = 6.0f;
constexpr auto WINDOW_COLOR = ImVec4(0.05f, 0.35f, 0.08f, 1.0f);

namespace game
{
  Loader::Loader()
  {
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
      std::cout << "Failed to initialize SDL: " << SDL_GetError();
      isError = true;
      return;
    }

    std::cout << "Initialized SDL" << "\n";

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_VERSION_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_VERSION_MINOR);
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("Snivy", SIZE.x, SIZE.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
      std::cout << "Failed to initialize window: " << SDL_GetError();
      ;
      isError = true;
      return;
    }

    std::cout << "Initialized window" << "\n";

    context = SDL_GL_CreateContext(window);

    if (!context)
    {
      std::cout << "Failed to initialize GL context: " << SDL_GetError();
      isError = true;
      return;
    }

    if (!SDL_GL_MakeCurrent(window, context))
    {
      std::cout << "Failed to make GL context current: " << SDL_GetError();
      isError = true;
      return;
    }

#ifndef __EMSCRIPTEN__
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
      std::cout << "Failed to initialize GLAD" << "\n";
      isError = true;
      return;
    }

    std::cout << "Initialized GLAD" << "\n";
#endif

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    std::cout << "Initialized GL context: " << glGetString(GL_VERSION) << "\n";

    SDL_GL_SetSwapInterval(1);

    if (!MIX_Init())
    {
      std::cout << "Failed to initialize SDL mixer: " << SDL_GetError();
      isError = true;
      return;
    }

    std::cout << "Initialized SDL mixer" << "\n";

    IMGUI_CHECKVERSION();
    ImGuiContext* imguiContext = ImGui::CreateContext();

    if (!imguiContext)
    {
      std::cout << "Failed to initialize Dear ImGui" << "\n";
      isError = true;
      return;
    }

    std::cout << "Initialized Dear ImGui" << "\n";

    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    io.Fonts->AddFontFromFileTTF("resources/font/pmd.ttf");
    style.WindowRounding = WINDOW_ROUNDING;
    style.ChildRounding = style.WindowRounding;
    style.FrameRounding = style.WindowRounding;
    style.GrabRounding = style.WindowRounding;
    style.PopupRounding = style.WindowRounding;
    style.ScrollbarRounding = style.WindowRounding;
    style.Colors[ImGuiCol_TitleBg] = WINDOW_COLOR;
    style.Colors[ImGuiCol_TitleBgActive] = WINDOW_COLOR;
    style.Colors[ImGuiCol_TitleBgCollapsed] = WINDOW_COLOR;

    io.IniFilename = nullptr;

    if (!ImGui_ImplSDL3_InitForOpenGL(window, context))
    {
      std::cout << "Failed to initialize Dear ImGui SDL3 backend" << "\n";
      ImGui::DestroyContext(imguiContext);
      isError = true;
      return;
    }

    std::cout << "Initialize Dear ImGui SDL3 backend" << "\n";

    if (!ImGui_ImplOpenGL3_Init(GLSL_VERSION))
    {
      std::cout << "Failed to initialize Dear ImGui OpenGL backend" << "\n";
      ImGui_ImplSDL3_Shutdown();
      ImGui::DestroyContext(imguiContext);
      isError = true;
      return;
    }

    std::cout << "Initialize Dear ImGui OpenGL backend" << "\n";
  }

  Loader::~Loader()
  {
    if (ImGui::GetCurrentContext())
    {
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplSDL3_Shutdown();
      ImGui::DestroyContext();
    }

    if (context) SDL_GL_DestroyContext(context);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Exiting..." << "\n";
  }
};
