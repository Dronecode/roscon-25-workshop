#pragma once

// PX4 Interface Library
#include <px4_ros2/components/mode.hpp>
#include <px4_ros2/utils/geometry.hpp>
#include <px4_ros2/odometry/local_position.hpp>
#include <px4_ros2/odometry/attitude.hpp>
#include <px4_ros2/odometry/angular_velocity.hpp>
#include <px4_msgs/msg/trajectory_setpoint.hpp>
#include <px4_ros2/control/setpoint_types/experimental/trajectory.hpp>
#include <px4_msgs/msg/vehicle_land_detected.hpp>

// ROS 2 Core
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

// C++ Std
#include <cmath>  // for M_PI
#include <Eigen/Eigen>
#include <mutex>
#include <string>

// Base class declaration for Search and Rescue (SAR) Mode
class BaseSARMode : public px4_ros2::ModeBase {
public:
    BaseSARMode(rclcpp::Node& node, const std::string& mode_name);
    ~BaseSARMode() override = default;

    void onActivate() override;
    void onDeactivate() override;

protected:
    rclcpp::Node& _node;
    const std::string _mode_name;
    std::shared_ptr<px4_ros2::TrajectorySetpointType> _trajectory_setpoint;
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr _target_sub;
    Eigen::Vector3f _target_pos{0.0f, 0.0f, 0.0f};
    float _target_yaw{0.0f};  // Rover heading, NED convention: 0 = North, +pi/2 = East
    bool _has_target{false};
    float _elapsed_time{0.0f};
};

// VSweep Formation Mode
class SARVSweepMode : public BaseSARMode {
public:
    explicit SARVSweepMode(rclcpp::Node& node, int drone_id=0, int total_drones=3);
    ~SARVSweepMode() override = default;

    void updateSetpoint(float dt) override;

private:
    int _drone_id;
    int _total_drones;
};


// Orbital Formation Mode
class SAROrbitalMode : public BaseSARMode {
public:
    explicit SAROrbitalMode(rclcpp::Node& node, int drone_id=0, int total_drones=3);
    ~SAROrbitalMode() override = default;

    void updateSetpoint(float dt) override;

private:
    int _drone_id;
    int _total_drones;
    float _radius{14.0f};
    float _omega{0.4f};
    std::mutex _param_mutex;  // guards _radius/_omega for a future dynamic-reconfigure callback
};

// The "SAR (Auto)" in QGC is the executor which immediately
// reschedules into SARVSweepMode or SAROrbitalMode based on rover speed.
class SARAutoMode : public px4_ros2::ModeBase {
public:
    explicit SARAutoMode(rclcpp::Node& node);
    ~SARAutoMode() override = default;

    void onActivate() override;
    void onDeactivate() override;

private:
    rclcpp::Node& _node;

    // Little hack (?): ModeBase requires at least one setpoint type configured before it can
    // register/activate, even though this mode never actually issues one.
    std::shared_ptr<px4_ros2::TrajectorySetpointType> _trajectory_setpoint;
};
