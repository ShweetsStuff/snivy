#pragma once

namespace game::util::imgui
{
  class WindowSlide
  {
  public:
    explicit WindowSlide(float duration = 0.125f, float initial = 1.0f);

    void update(bool isOpen, float deltaTime);
    float value_get() const;
    float eased_get(float power = 2.0f) const;
    bool is_visible() const;

  private:
    float value_{1.0f};
    float duration_{0.125f};
  };
}
