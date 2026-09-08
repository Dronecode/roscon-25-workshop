#include <chrono>

#include "MousePointerGimbalControl.hpp"

using namespace std::chrono_literals;

namespace {
constexpr auto kControlPeriod = 20ms;  // 50 Hz publish loop, matches gimbal_teleop's cmd_vel rate
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
