#include "camera.h"

using namespace glm;
using namespace game::resource;
using namespace game::util;

namespace game
{
  void Camera::on_resize(int width, int height)
  {
    float aspectRatio = width / (float)height;
    float bgAspectRatio = targetSize.x / (float)targetSize.y;

    if (aspectRatio > bgAspectRatio)
    {
      actualSize.x = (int)(height * bgAspectRatio);
      actualSize.y = height;

      cropScale = height / targetSize.y;
    }
    else
    {
      actualSize.x = width;
      actualSize.y = (int)(width / bgAspectRatio);

      cropScale = width / targetSize.x;
    }

    cropOffset.x = (width - actualSize.x) / 2;
    cropOffset.y = (height - actualSize.y) / 2;
  }

  void Camera::update()
  {
    if (ImGui::IsKeyDown(ImGuiKey_UpArrow) || ImGui::IsKeyDown(ImGuiKey_W))
    {
      cameraPosition.y -= 1;
    }

    if (ImGui::IsKeyDown(ImGuiKey_DownArrow) || ImGui::IsKeyDown(ImGuiKey_S))
    {
      cameraPosition.y += 1;
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftArrow) || ImGui::IsKeyDown(ImGuiKey_A))
    {
      cameraPosition.x -= 1;
    }

    if (ImGui::IsKeyDown(ImGuiKey_RightArrow) || ImGui::IsKeyDown(ImGuiKey_D))
    {
      cameraPosition.x += 1;
    }
  }

  mat4 Camera::get_model(glm::vec2 const size, glm::vec2 const position, glm::vec2 const pivot, glm::vec2 const scale,
                         float rotation)
  {
    auto canvasPosition = to_screen(position);

    glm::mat4 model = math::quad_model_get(size, canvasPosition, pivot, scale * cropScale, rotation);

    return model;
  }

  //TODO: Review if this is even needed
  mat4 Camera::get_root_model(glm::vec2 const position, glm::vec2 const pivot, glm::vec2 const scale, float rotation)
  {
    auto canvasPosition = to_screen(position);

    auto model = math::quad_model_parent_get(canvasPosition, pivot, scale * cropScale, rotation);

    return model;
  }

  ivec4 Camera::get_bounds(vec2 size, vec2 position)
  {
    auto canvasPosition = to_screen(position);

    return ivec4(canvasPosition.x, canvasPosition.y, canvasPosition.x + size.x * cropScale,
                 canvasPosition.y + size.y * cropScale);
  }

  ivec2 Camera::to_screen(vec2 world) { return (world + cameraPosition) / targetSize * actualSize + cropOffset; }
  ivec2 Camera::to_world(vec2 screen) { return (screen - cropOffset) / actualSize * targetSize - cameraPosition; }
}
