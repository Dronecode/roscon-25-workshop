#pragma once

#include <px4_ros2/components/mode_executor.hpp>
#include <px4_ros2/components/node_with_mode.hpp>

#include "Teleop.hpp"

class TeleopExecutor : public px4_ros2::ModeExecutorBase {
public:
    TeleopExecutor(rclcpp::Node &node, px4_ros2::ModeBase &owned_mode);

    // See ModeExecutorBase
    void onActivate() override;
    void onDeactivate(DeactivateReason reason) override;

private:
    // ROS2
    rclcpp::Node &_node;

    // State management
    enum class State {
        Takeoff,           // Initial state, takeoff to a predefined altitude
        TeleOperation,     // Custom teleoperation mode
        RTL,               // Return to Launch state
        Land,              // Land state
        WaitUntilDisarmed  // Final state, wait until the vehicle is disarmed
    };
    State _state;
    void switchToState(State state, px4_ros2::Result previous_result);
};
