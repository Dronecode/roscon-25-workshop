#include "CustomMode.hpp"

#include <px4_ros2/components/node_with_mode.hpp>

static const std::string kModeNameCustomWaypoints = "CustomWaypoints";
static const std::string kNodeName = "custom_mode_demo";
static const bool kEnableDebugOutput = true;

CustomWaypoints::CustomWaypoints(rclcpp::Node& node)
    : px4_ros2::ModeBase(node, kModeNameCustomWaypoints), _node(node)
{
  loadParameters();

  _trajectory_setpoint = std::make_shared<px4_ros2::TrajectorySetpointType>(*this);
  _local_position = std::make_shared<px4_ros2::OdometryLocalPosition>(*this);

  RCLCPP_INFO(node.get_logger(), "CustomWaypoints mode initialized.");
}

void CustomWaypoints::loadParameters()
{
}

void CustomWaypoints::onActivate()
{
  // Initialize waypoints

  _trajectory_waypoints.push_back(Eigen::Vector3f(5.0f, 0.0f, -1.5f));
  _trajectory_waypoints.push_back(Eigen::Vector3f(5.0f, 5.0f, -1.5f));
  _trajectory_waypoints.push_back(Eigen::Vector3f(-5.0f, 5.0f, -1.5f));
  _trajectory_waypoints.push_back(Eigen::Vector3f(-5.0f, -5.0f, -1.5f));
  _trajectory_waypoints.push_back(Eigen::Vector3f(5.0f, -5.0f, -1.5f));
  _trajectory_waypoints.push_back(Eigen::Vector3f(5.0f, 0.0f, -1.5f));
  _trajectory_waypoints.push_back(Eigen::Vector3f(0.0f, 0.0f, -1.5f));

  _current_waypoint_index = 0;
  RCLCPP_INFO(_node.get_logger(), "CustomWaypoints mode activated");
}
void CustomWaypoints::onDeactivate()
{
  RCLCPP_INFO(_node.get_logger(), "CustomWaypoints mode deactivated");
}
void CustomWaypoints::updateSetpoint([[maybe_unused]] float dt_s)
{
  if (_current_waypoint_index < _trajectory_waypoints.size()) {
    auto current_waypoint = _trajectory_waypoints[_current_waypoint_index];
    _trajectory_setpoint->updatePosition(current_waypoint);

    if ((_local_position->positionNed() - current_waypoint).norm() < 0.5f) {
      _current_waypoint_index++;
    }
  } else {
    RCLCPP_INFO_ONCE(_node.get_logger(), "All waypoints completed.");
    completed(px4_ros2::Result::Success);
    return;
  }
}

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(
      std::make_shared<px4_ros2::NodeWithMode<CustomWaypoints>>(kNodeName, kEnableDebugOutput));
  rclcpp::shutdown();
  return 0;
}
