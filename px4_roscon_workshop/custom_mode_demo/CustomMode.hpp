#pragma once

#include <px4_ros2/components/mode.hpp>
#include <px4_ros2/odometry/local_position.hpp>
#include <px4_ros2/control/setpoint_types/experimental/trajectory.hpp>

#include <rclcpp/rclcpp.hpp>

#include <Eigen/Eigen>
#include <memory>
#include <vector>

class CustomWaypoints : public px4_ros2::ModeBase {
public:
    explicit CustomWaypoints(rclcpp::Node &node);

    // See ModeBase
    void onActivate() override;
    void onDeactivate() override;
    void updateSetpoint([[maybe_unused]] float dt_s) override;

private:
    void loadParameters();
    rclcpp::Node &_node;

    std::shared_ptr<px4_ros2::TrajectorySetpointType> _trajectory_setpoint;
    std::shared_ptr<px4_ros2::OdometryLocalPosition> _local_position;

    std::vector<Eigen::Vector3f> _trajectory_waypoints;
    size_t _current_waypoint_index{};
};
