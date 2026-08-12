// ============================================================================
// ORIGINAL VERSION - CustomModeExecutor_v2.hpp
// ============================================================================
#pragma once

#include <px4_ros2/components/mode_executor.hpp>
#include <px4_ros2/components/node_with_mode.hpp>

#include "CustomMode_v2.hpp"

class CustomModeExecutor : public px4_ros2::ModeExecutorBase {
public:
    CustomModeExecutor(rclcpp::Node &node, px4_ros2::ModeBase &owned_mode,
                       px4_ros2::ModeBase &second_mode);

    // See ModeExecutorBase
    void onActivate() override;
    void onDeactivate(DeactivateReason reason) override;

private:
    // ROS2
    rclcpp::Node &_node;
    px4_ros2::ModeBase &_second_mode;

    // State management
    enum class State {
        Takeoff,           // Initial state, takeoff to a predefined altitude
        CustomWaypoints,     // Custom waypoints mode
        PrecisionLand,               // Precision landing mode
        WaitUntilDisarmed  // Final state, wait until the vehicle is disarmed
    };
    State _state;
    void switchToState(State state, px4_ros2::Result previous_result);
};