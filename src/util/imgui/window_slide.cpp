#include "window_slide.hpp"

#include <algorithm>
#include <cmath>

namespace game::util::imgui
{
  WindowSlide::WindowSlide(float duration, float initial) : value_(initial), duration_(duration) {}

  void WindowSlide::update(bool isOpen, float deltaTime)
  {
    auto target = isOpen ? 1.0f : 0.0f;
    if (duration_ <= 0.0f)
    {
      value_ = target;
      return;
    }

    auto step = deltaTime / duration_;
    if (value_ < target) value_ = std::min(target, value_ + step);
    if (value_ > target) value_ = std::max(target, value_ - step);
    value_ = std::clamp(value_, 0.0f, 1.0f);
  }

  float WindowSlide::value_get() const { return value_; }

  float WindowSlide::eased_get(float power) const { return std::pow(value_, power); }

  bool WindowSlide::is_visible() const { return value_ > 0.0f; }
}
