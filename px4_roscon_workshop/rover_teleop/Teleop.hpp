#pragma once

// PX4 Interface Library
#include <px4_ros2/components/mode.hpp>
#include <px4_ros2/control/setpoint_types/experimental/rover/throttle_steering.hpp>

// ROS 2 Core
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/bool.hpp>

// C++ Std
#include <chrono>

class Teleop : public px4_ros2::ModeBase {
public:
    explicit Teleop(rclcpp::Node &node);

    // See ModeBase
    void onActivate() override;
    void onDeactivate() override;
    void updateSetpoint([[maybe_unused]] float dt_s) override;

private:
    void loadParameters();
    // ROS 2
    rclcpp::Node &_node;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr _twist_sub;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr _active_sub;
    geometry_msgs::msg::Twist _last_twist;
    rclcpp::Time _last_twist_time;
    rclcpp::Clock::SharedPtr _clock;

    // px4_ros2_cpp
    std::shared_ptr<px4_ros2::RoverThrottleSteeringSetpointType> _rover_throttle_steering_setpoint;
    std::chrono::duration<double> _teleop_duration;
    bool _teleop_active;
};
