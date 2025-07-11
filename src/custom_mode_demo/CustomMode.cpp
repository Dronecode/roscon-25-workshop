#include "CustomMode.hpp"

#include <px4_ros2/components/node_with_mode.hpp>

static const std::string kModeNameCustomWaypoints = "CustomWaypoints";
static const std::string kModeNameCustomYaw = "CustomYaw";

CustomWaypoints::CustomWaypoints(rclcpp::Node &node)
    : px4_ros2::ModeBase(node, kModeNameCustomWaypoints),
      _node(node)
{
    loadParameters();

    _trajectory_setpoint = std::make_shared<px4_ros2::TrajectorySetpointType>(*this);
    _local_position = std::make_shared<px4_ros2::OdometryLocalPosition>(*this);

    RCLCPP_INFO(node.get_logger(), "CustomWaypoints mode initialized.");

}

CustomYaw::CustomYaw(rclcpp::Node &node)
    : px4_ros2::ModeBase(node, kModeNameCustomYaw),
      _node(node)
{
    loadParameters();

    _trajectory_setpoint = std::make_shared<px4_ros2::TrajectorySetpointType>(*this);
    _vehicle_attitude = std::make_shared<px4_ros2::OdometryAttitude>(*this);
    _local_position = std::make_shared<px4_ros2::OdometryLocalPosition>(*this);

    RCLCPP_INFO(node.get_logger(), "CustomYaw mode initialized.");
}

void CustomWaypoints::loadParameters() {
    // Load parameters specific to the CustomWaypoints mode
}
void CustomYaw::loadParameters() {
    // Load parameters specific to the CustomYaw mode
}

void CustomWaypoints::onActivate() {
    // Initialize waypoints
    _trajectory_waypoints.push_back(Eigen::Vector3f(5.0f, 0.0f, -2.0f)); // Initial waypoint at home position
    _trajectory_waypoints.push_back(Eigen::Vector3f(5.0f, 5.0f, -2.0f)); // Second waypoint
    _trajectory_waypoints.push_back(Eigen::Vector3f(-5.0f, 5.0f, -2.0f)); // Third waypoint
    _trajectory_waypoints.push_back(Eigen::Vector3f(-5.0f, -5.0f, -2.0f)); // Fourth waypoint
    _trajectory_waypoints.push_back(Eigen::Vector3f(_local_position->positionNed().x(),
                                                    _local_position->positionNed().y(),
                                                    _local_position->positionNed().z())); // Final waypoint at current position
    _current_waypoint_index = 0; // Start at the first waypoint
    RCLCPP_INFO(_node.get_logger(), "CustomWaypoints mode activated");
    // Set initial trajectory setpoint
}
void CustomWaypoints::onDeactivate() {
    RCLCPP_INFO(_node.get_logger(), "CustomWaypoints mode deactivated");
    // Reset trajectory setpoint
}
void CustomWaypoints::updateSetpoint([[maybe_unused]] float dt_s) {
    if (_current_waypoint_index < _trajectory_waypoints.size()) {
        // Set the trajectory setpoint to the current waypoint
        auto current_waypoint = _trajectory_waypoints[_current_waypoint_index];
        _trajectory_setpoint->updatePosition(current_waypoint);


        // Check if we reached the current waypoint
        if ((_local_position->positionNed() - current_waypoint).norm() < 0.5f) {
            _current_waypoint_index++; // Move to the next waypoint
        }
    } else {
        // All waypoints completed, reset or stop
        RCLCPP_INFO(_node.get_logger(), "All waypoints completed.");
        completed(px4_ros2::Result::Success);
        return; // Exit the update loop
    }
    
}
void CustomYaw::onActivate() {
    _start_yaw = _vehicle_attitude->yaw(); // Store the starting yaw angle
    _yaw_accumulator = 0.0f; // Initialize yaw accumulator
    RCLCPP_INFO(_node.get_logger(), "CustomYaw mode activated");
    // Set initial trajectory setpoint
}
void CustomYaw::onDeactivate() {
    RCLCPP_INFO(_node.get_logger(), "CustomYaw mode deactivated");
    // Reset trajectory setpoint
}
void CustomYaw::updateSetpoint([[maybe_unused]] float dt_s) {
    // Update the trajectory setpoint based on the current heading
    Eigen::Vector3f velocity{0.0f, 0.0f, 0.0f};
    std::optional<Eigen::Vector3f> acceleration = std::nullopt;
    std::optional<float> yaw = std::nullopt;
    std::optional<float> yaw_rate = 0.05f;
    _trajectory_setpoint->update(velocity, acceleration, yaw, yaw_rate);
    _yaw_accumulator += yaw_rate.value() * dt_s; // Accumulate yaw rotation
    if (std::abs(_yaw_accumulator) > 2 * M_PI - 0.1f) {  // full rotation (tolerant)
        RCLCPP_INFO(_node.get_logger(), "CustomYaw mode completed a full rotation.");
        completed(px4_ros2::Result::Success);
        return;
    }
}
