#pragma once

// PX4 Interface Library
#include <px4_ros2/components/mode.hpp>
#include <px4_ros2/utils/geometry.hpp>
#include <px4_ros2/odometry/local_position.hpp>
#include <px4_ros2/odometry/attitude.hpp>
#include <px4_ros2/odometry/angular_velocity.hpp>
#include <px4_msgs/msg/trajectory_setpoint.hpp>
#include <px4_ros2/control/setpoint_types/experimental/trajectory.hpp>

// ROS 2 Core
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/bool.hpp>

// C++ Std
#include <cmath>  // for M_PI
#include <Eigen/Eigen>
#include <chrono>

class CustomWaypoints : public px4_ros2::ModeBase {
public:
    explicit CustomWaypoints(rclcpp::Node &node);

    // See ModeBase
    void onActivate() override;
    void onDeactivate() override;
    void updateSetpoint([[maybe_unused]] float dt_s) override;

private:
    void loadParameters();
    // ROS 2
    rclcpp::Node &_node;


    // px4_ros2_cpp
    std::shared_ptr<px4_ros2::TrajectorySetpointType> _trajectory_setpoint;
    std::shared_ptr<px4_ros2::OdometryLocalPosition> _local_position;

    std::vector<Eigen::Vector3f> _trajectory_waypoints; // Vector to hold waypoints
    size_t _current_waypoint_index; // Index of the current waypoint
};

class CustomYaw : public px4_ros2::ModeBase {
public:
    explicit CustomYaw(rclcpp::Node &node);

    // See ModeBase
    void onActivate() override;
    void onDeactivate() override;
    void updateSetpoint([[maybe_unused]] float dt_s) override;

private:
    void loadParameters();
    // ROS 2
    rclcpp::Node &_node;


    // px4_ros2_cpp
    std::shared_ptr<px4_ros2::OdometryAttitude> _vehicle_attitude;
    std::shared_ptr<px4_ros2::TrajectorySetpointType> _trajectory_setpoint;
    std::shared_ptr<px4_ros2::OdometryLocalPosition> _local_position;
    

    float _start_yaw; // Starting yaw angle
    float _yaw_accumulator; // Increment for yaw rotation
};

class CustomAltitude : public px4_ros2::ModeBase {
public:
    explicit CustomAltitude(rclcpp::Node &node);

    // See ModeBase
    void onActivate() override;
    void onDeactivate() override;
    void updateSetpoint([[maybe_unused]] float dt_s) override;

private:
    void loadParameters();
    // ROS 2
    rclcpp::Node &_node;


    // px4_ros2_cpp
    std::shared_ptr<px4_ros2::TrajectorySetpointType> _trajectory_setpoint;
    std::shared_ptr<px4_ros2::OdometryLocalPosition> _local_position;
};
