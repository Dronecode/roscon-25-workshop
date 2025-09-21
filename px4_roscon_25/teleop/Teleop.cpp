#include "Teleop.hpp"

#include <px4_ros2/components/node_with_mode.hpp>

static const std::string kModeName = "Teleoperation";
static const bool kEnableDebug = true;

Teleop::Teleop(rclcpp::Node& node)
    : px4_ros2::ModeBase(node, kModeName)
    , _node(node)
{
    _trajectory_setpoint = std::make_shared<px4_ros2::TrajectorySetpointType>(*this);
    _vehicle_attitude = std::make_shared<px4_ros2::OdometryAttitude>(*this);
    _clock = std::make_shared<rclcpp::Clock>(RCL_SYSTEM_TIME);
    loadParameters();
    _twist_sub = _node.create_subscription<geometry_msgs::msg::Twist>(
        "/cmd_vel", 10,
        [this](const geometry_msgs::msg::Twist::SharedPtr msg) {
            _last_twist = *msg;
            _last_twist_time = _clock->now();
        });
    _active_sub = _node.create_subscription<std_msgs::msg::Bool>(
        "/teleop/active", 10,
        [this](const std_msgs::msg::Bool::SharedPtr msg) {
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
        RCLCPP_WARN(_node.get_logger(), "Invalid teleop_duration (%f), using default 60.0", duration_sec);
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
        RCLCPP_WARN(_node.get_logger(), "Teleop keyboard was closed or no Twist commands for %.0f seconds, exiting Teleop mode.", _teleop_duration.count());
        completed(px4_ros2::Result::Success);
        return;
    }

    // Default values: zero velocity, no acceleration, no yaw input
    Eigen::Vector3f velocity_ned{0.f, 0.f, 0.f};
    std::optional<Eigen::Vector3f> acceleration = std::nullopt;
    std::optional<float> yaw = std::nullopt;
    std::optional<float> yaw_rate = std::nullopt;

    if ((now - _last_twist_time).seconds() <= 0.2) {
        // Convert Twist (assumed ENU) to NED
        // ENU: x=forward, y=left, z=up → NED: x=forward, y=right, z=down
        const geometry_msgs::msg::Twist &twist = _last_twist;
        float yaw = _vehicle_attitude->yaw(); // Get current yaw from attitude
        // Convert to NED frame
        Eigen::Vector3f velocity_body;
        velocity_body.x() = twist.linear.x;
        velocity_body.y() = twist.linear.y;
        velocity_body.z() = twist.linear.z;

        velocity_ned.x() = velocity_body.x() * cos(yaw) - velocity_body.y() * sin(yaw);
        velocity_ned.y() = velocity_body.x() * sin(yaw) + velocity_body.y() * cos(yaw);
        velocity_ned.z() = velocity_body.z();

        yaw_rate = -twist.angular.z; // ENU and NED both define yaw CCW from north
    }

    _trajectory_setpoint->update(velocity_ned, acceleration, yaw, yaw_rate);
}
