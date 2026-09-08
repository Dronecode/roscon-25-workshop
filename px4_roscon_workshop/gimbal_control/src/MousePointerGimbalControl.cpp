#include "MousePointerGimbalControl.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

using namespace std::chrono_literals;

namespace {
constexpr auto kControlPeriod = 20ms;  // 50 Hz publish loop, matches gimbal_teleop's cmd_vel rate

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

MousePointerGimbalControlNode::MousePointerGimbalControlNode()
    : rclcpp::Node("mouse_pointer_gimbal_control_node")
{
  loadParameters();

  _last_point_time = now();

  _point_sub = create_subscription<geometry_msgs::msg::PointStamped>(
      "mouse/position", 10,
      [this](const geometry_msgs::msg::PointStamped::SharedPtr msg) { pointCallback(msg); });

  _camera_info_sub = create_subscription<sensor_msgs::msg::CameraInfo>(
      "camera_info", rclcpp::SensorDataQoS(),
      [this](const sensor_msgs::msg::CameraInfo::SharedPtr msg) { cameraInfoCallback(msg); });

  _cmd_vel_pub = create_publisher<geometry_msgs::msg::Twist>("gimbal/cmd_vel", 10);

  _control_timer = create_wall_timer(kControlPeriod, [this]() { controlLoop(); });
}

void MousePointerGimbalControlNode::loadParameters()
{
  declare_parameter<double>("max_pan_rate", 60.0);
  declare_parameter<double>("max_tilt_rate", 60.0);
  declare_parameter<double>("deadzone", 0.05);
  declare_parameter<double>("pointer_timeout", 0.5);
  declare_parameter<bool>("invert_tilt", true);

  get_parameter("max_pan_rate", _param_max_pan_rate);
  get_parameter("max_tilt_rate", _param_max_tilt_rate);
  get_parameter("deadzone", _param_deadzone);
  get_parameter("pointer_timeout", _param_pointer_timeout);
  get_parameter("invert_tilt", _param_invert_tilt);

  RCLCPP_INFO(get_logger(),
              "max_pan_rate=%.3f deg/s max_tilt_rate=%.3f deg/s deadzone=%.3f "
              "pointer_timeout=%.3f s invert_tilt=%s",
              _param_max_pan_rate, _param_max_tilt_rate, _param_deadzone, _param_pointer_timeout,
              _param_invert_tilt ? "true" : "false");
}

void MousePointerGimbalControlNode::pointCallback(
    const geometry_msgs::msg::PointStamped::SharedPtr msg)
{
  _last_point = msg;
  _last_point_time = now();
}

void MousePointerGimbalControlNode::cameraInfoCallback(
    const sensor_msgs::msg::CameraInfo::SharedPtr msg)
{
  _image_width = msg->width;
  _image_height = msg->height;
}

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
