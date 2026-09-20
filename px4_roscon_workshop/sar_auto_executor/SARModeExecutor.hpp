#pragma once

#include "SARMode.hpp"
#include <px4_ros2/components/mode_executor.hpp>

// Automatically switches between SAR (V-Sweep) and SAR (Orbital) based on the
// tracked rover's speed, computed from consecutive target's pose messages.
class SARModeExecutor : public px4_ros2::ModeExecutorBase {
public:
    SARModeExecutor(rclcpp::Node& node, px4_ros2::ModeBase& owned_mode,
                     px4_ros2::ModeBase& vsweep_mode, px4_ros2::ModeBase& orbital_mode);
    ~SARModeExecutor() override = default;

    void onActivate() override;
    void onDeactivate(DeactivateReason reason) override;

private:
    void targetPoseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr msg);
    void evaluateAndSwitch();

    rclcpp::Node& _node;
    px4_ros2::ModeBase& _vsweep_mode;
    px4_ros2::ModeBase& _orbital_mode;

    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr _target_sub;

    bool _has_prev{false};
    float _prev_x{0.0f};
    float _prev_y{0.0f};
    rclcpp::Time _prev_stamp;
    float _current_speed{0.0f};

    // Below kSpeedLowThreshold -> Orbital, at/above kSpeedHighThreshold -> V-Sweep,
    // with hysteresis in between to avoid rapid mode flapping.
    static constexpr float kSpeedLowThreshold{0.3f};   // below this -> Orbital
    static constexpr float kSpeedHighThreshold{0.6f};  // at/above this -> V-Sweep

    px4_ros2::ModeBase::ModeID _active_mode_id{px4_ros2::ModeBase::kModeIDInvalid};
};
