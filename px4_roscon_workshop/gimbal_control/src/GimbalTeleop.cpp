#include "GimbalTeleop.hpp"

#include <algorithm>
#include <chrono>

using namespace std::chrono_literals;

namespace {
constexpr auto kControlPeriod = 20ms;     // 50 Hz integration/publish loop
constexpr double kTwistTimeoutSec = 0.5;  // stop moving if no Twist for this long
}  // namespace

GimbalTeleopNode::GimbalTeleopNode() : rclcpp::Node("gimbal_teleop_node")
{
  loadParameters();

  _last_twist_time = now();
  _last_update_time = now();

  _twist_sub = create_subscription<geometry_msgs::msg::Twist>(
      "gimbal/cmd_vel", 10,
      [this](const geometry_msgs::msg::Twist::SharedPtr msg) { twistCallback(msg); });

  auto qos = rclcpp::QoS(1).best_effort();
  _gimbal_controls_pub =
      create_publisher<px4_msgs::msg::GimbalControls>("fmu/in/gimbal_controls", qos);

  _control_timer = create_wall_timer(kControlPeriod, [this]() { controlLoop(); });
}

void GimbalTeleopNode::loadParameters()
{
  declare_parameter<double>("pan_gain", 1.0);
  declare_parameter<double>("tilt_gain", 1.0);
  declare_parameter<double>("yaw_limit", 180.0);
  declare_parameter<double>("pitch_min", -90.0);
  declare_parameter<double>("pitch_max", 45.0);

  get_parameter("pan_gain", _param_pan_gain);
  get_parameter("tilt_gain", _param_tilt_gain);
  get_parameter("yaw_limit", _param_yaw_limit);
  get_parameter("pitch_min", _param_pitch_min);
  get_parameter("pitch_max", _param_pitch_max);

  RCLCPP_INFO(get_logger(),
              "pan_gain=%.3f tilt_gain=%.3f yaw_limit=%.3f deg pitch_min=%.3f deg pitch_max=%.3f "
              "deg",
              _param_pan_gain, _param_tilt_gain, _param_yaw_limit, _param_pitch_min,
              _param_pitch_max);
}

void GimbalTeleopNode::twistCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
{
  // Positive linear.x pans right (positive yaw rate), positive linear.y pitches up.
  _pan_rate = _param_pan_gain * msg->linear.x;
  _tilt_rate = _param_tilt_gain * msg->linear.y;
  _last_twist_time = now();
}

void GimbalTeleopNode::controlLoop()
{
  const rclcpp::Time current_time = now();
  const double dt = (current_time - _last_update_time).seconds();
  _last_update_time = current_time;

  const bool twist_is_stale = (current_time - _last_twist_time).seconds() > kTwistTimeoutSec;
  const double pan_rate = twist_is_stale ? 0.0 : _pan_rate;
  const double tilt_rate = twist_is_stale ? 0.0 : _tilt_rate;

  _yaw_angle = std::clamp(_yaw_angle + pan_rate * dt, -_param_yaw_limit, _param_yaw_limit);
  _pitch_angle = std::clamp(_pitch_angle + tilt_rate * dt, _param_pitch_min, _param_pitch_max);

  // Normalize to the [-1, 1] range expected by GimbalControls: yaw is symmetric around
  // zero, while pitch maps its (possibly asymmetric) [min, max] range linearly.
  const double normalized_yaw =
      _param_yaw_limit > 0.0 ? std::clamp(_yaw_angle / _param_yaw_limit, -1.0, 1.0) : 0.0;
  const double pitch_range = _param_pitch_max - _param_pitch_min;
  const double normalized_pitch =
      pitch_range > 0.0
          ? std::clamp(2.0 * (_pitch_angle - _param_pitch_min) / pitch_range - 1.0, -1.0, 1.0)
          : 0.0;

  px4_msgs::msg::GimbalControls msg{};
  msg.timestamp = static_cast<uint64_t>(current_time.nanoseconds() / 1000);
  msg.timestamp_sample = msg.timestamp;
  msg.control[px4_msgs::msg::GimbalControls::INDEX_ROLL] = 0.0f;
  msg.control[px4_msgs::msg::GimbalControls::INDEX_PITCH] = static_cast<float>(normalized_pitch);
  msg.control[px4_msgs::msg::GimbalControls::INDEX_YAW] = static_cast<float>(normalized_yaw);

  _gimbal_controls_pub->publish(msg);
}

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<GimbalTeleopNode>());
  rclcpp::shutdown();
  return 0;
}
