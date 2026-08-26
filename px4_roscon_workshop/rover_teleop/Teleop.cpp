#include "Teleop.hpp"

#include <algorithm>  // for std::clamp
#include <px4_ros2/components/node_with_mode.hpp>

static const std::string kModeName = "Teleoperation";
static const bool kEnableDebug = true;

Teleop::Teleop(rclcpp::Node& node) : px4_ros2::ModeBase(node, kModeName), _node(node)
{
  _rover_throttle_steering_setpoint =
      std::make_shared<px4_ros2::RoverThrottleSteeringSetpointType>(*this);
  _clock = std::make_shared<rclcpp::Clock>(RCL_SYSTEM_TIME);
  loadParameters();
  _twist_sub = _node.create_subscription<geometry_msgs::msg::Twist>(
      "/cmd_vel", 10, [this](const geometry_msgs::msg::Twist::SharedPtr msg) {
        _last_twist = *msg;
        _last_twist_time = _clock->now();
      });
  _active_sub = _node.create_subscription<std_msgs::msg::Bool>(
      "/teleop/active", 10, [this](const std_msgs::msg::Bool::SharedPtr msg) {
        _teleop_active = msg->data;
        RCLCPP_INFO(_node.get_logger(), "Teleop active: %s", _teleop_active ? "true" : "false");
      });
}
void Teleop::loadParameters()
{
  _node.declare_parameter<double>("teleop_duration", 60.0);
  double duration_sec = _node.get_parameter("teleop_duration").as_double();

  RCLCPP_INFO(_node.get_logger(), "Teleoperation duration set to: %.2f seconds", duration_sec);

  // Ensure the value is sane
  if (duration_sec < 5.0) {
    RCLCPP_WARN(_node.get_logger(), "Invalid teleop_duration (%f), using default 60.0",
                duration_sec);
    duration_sec = 60.0;
  }

  _teleop_duration = std::chrono::duration<double>(duration_sec);
}

void Teleop::onActivate()
{
  _last_twist_time = _clock->now();
  _teleop_active = true;
  RCLCPP_INFO(_node.get_logger(), "Teleop mode activated");
}

void Teleop::onDeactivate()
{
  RCLCPP_INFO(_node.get_logger(), "Teleop mode deactivated");
}

void Teleop::updateSetpoint([[maybe_unused]] float dt_s)
{
  const auto now = _clock->now();

  if (((now - _last_twist_time) > _teleop_duration) || (_teleop_active == false)) {
    RCLCPP_WARN(
        _node.get_logger(),
        "Teleop keyboard was closed or no Twist commands for %.0f seconds, exiting Teleop mode.",
        _teleop_duration.count());
    completed(px4_ros2::Result::Success);
    return;
  }

  // Default values: no throttle, no steering
  float throttle_body_x = 0.f;
  float normalized_steering_setpoint = 0.f;

  if ((now - _last_twist_time).seconds() <= 0.2) {
    const geometry_msgs::msg::Twist& twist = _last_twist;

    // Twist.linear.x maps directly to the forward/backward throttle setpoint.
    throttle_body_x = std::clamp(static_cast<float>(twist.linear.x), -1.f, 1.f);

    // For a differential rover the steering setpoint is the normalized speed
    // difference between the left and right wheels, not a steering angle.
    // Twist.angular.z is CCW-positive (left turn), while the steering setpoint
    // is positive to the right, hence the sign flip.
    normalized_steering_setpoint = std::clamp(static_cast<float>(-twist.angular.z), -1.f, 1.f);
  }

  _rover_throttle_steering_setpoint->update(throttle_body_x, normalized_steering_setpoint);
}

using TeleopNodeWithMode = px4_ros2::NodeWithMode<Teleop>;

static const std::string kNodeName = "teleop_node";

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TeleopNodeWithMode>(kNodeName, kEnableDebug));
  rclcpp::shutdown();
  return 0;
}
