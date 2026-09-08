#include <algorithm>
#include <cmath>

#include "MousePointerGimbalControl.hpp"
#include "rclcpp/rclcpp.hpp"

namespace {
// Rescales |value| from [deadzone, 1] to [0, 1], zeroing anything inside the deadzone, and
// preserves the original sign.
double applyDeadzone(double value, double deadzone)
{
  const double magnitude = std::abs(value);
  if (magnitude <= deadzone || deadzone >= 1.0) {
    return 0.0;
  }
  const double scaled = (magnitude - deadzone) / (1.0 - deadzone);
  return std::copysign(scaled, value);
}
}  // namespace

void MousePointerGimbalControlNode::controlLoop()
{
  const rclcpp::Time current_time = now();

  const bool have_image_size = _image_width > 0 && _image_height > 0;
  const bool have_point = _last_point != nullptr;
  const bool pointer_is_stale =
      (current_time - _last_point_time).seconds() > _param_pointer_timeout;

  double pan_rate = 0.0;
  double tilt_rate = 0.0;

  if (have_image_size && have_point && !pointer_is_stale) {
    // Normalize pointer position to [-1, 1] around the image center, with +x to the right and
    // +y downward (image convention).
    const double half_width = static_cast<double>(_image_width) / 2.0;
    const double half_height = static_cast<double>(_image_height) / 2.0;
    const double norm_x = std::clamp((_last_point->point.x - half_width) / half_width, -1.0, 1.0);
    const double norm_y = std::clamp((_last_point->point.y - half_height) / half_height, -1.0, 1.0);

    const double pan_input = applyDeadzone(norm_x, _param_deadzone);
    // Image y grows downward, so a pointer above center (norm_y < 0) should tilt up by default.
    const double tilt_input = applyDeadzone(_param_invert_tilt ? -norm_y : norm_y, _param_deadzone);

    pan_rate = pan_input * _param_max_pan_rate;
    tilt_rate = tilt_input * _param_max_tilt_rate;
  }

  geometry_msgs::msg::Twist msg{};
  msg.linear.x = pan_rate;
  msg.linear.y = tilt_rate;
  _cmd_vel_pub->publish(msg);
}

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MousePointerGimbalControlNode>());
  rclcpp::shutdown();
  return 0;
}
