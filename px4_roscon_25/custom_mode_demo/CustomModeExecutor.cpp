// ============================================================================
// ORIGINAL VERSION - CustomModeExecutor.cpp
// ============================================================================
#include "CustomModeExecutor.hpp"

using CustomModeWithExecutor = px4_ros2::NodeWithModeExecutor<CustomModeExecutor, CustomWaypoints, CustomYaw>;

static const std::string kNodeName = "CustomModeDemo";
static const bool kEnableDebugOutput = true;

CustomModeExecutor::CustomModeExecutor(rclcpp::Node &node, px4_ros2::ModeBase &owned_mode, px4_ros2::ModeBase &second_mode)
    : ModeExecutorBase(node, Settings{}, owned_mode), _node(node), _second_mode(second_mode) {}

void CustomModeExecutor::onActivate() {
    RCLCPP_INFO(_node.get_logger(), "CustomModeExecutor activated");
    switchToState(State::Takeoff, px4_ros2::Result::Success);
}

void CustomModeExecutor::onDeactivate(DeactivateReason reason) {
    const char *reason_str = (reason == DeactivateReason::FailsafeActivated)
                                 ? "failsafe activated"
                                 : "other reason";
    RCLCPP_INFO(_node.get_logger(), "CustomModeExecutor deactivated: %s", reason_str);
}

void CustomModeExecutor::switchToState(State state, px4_ros2::Result previous_result) {
    _state = state;
    if (previous_result != px4_ros2::Result::Success) {
        RCLCPP_WARN(_node.get_logger(),
                    "Switching to state %d due to previous result: %d",
                    static_cast<int>(state), static_cast<int>(previous_result));
    }

    RCLCPP_INFO(_node.get_logger(), "Switched to state: %d", static_cast<int>(state));

    // Handle state-specific logic here
    switch (state) {
        case State::Takeoff:
            RCLCPP_INFO(_node.get_logger(), "Initiating takeoff...");
            takeoff(
                [this](px4_ros2::Result result) {
                    switchToState(State::CustomWaypoints, result);
                },
                2.0f);
            break;
        case State::CustomWaypoints:
            scheduleMode(ownedMode().id(), [this](px4_ros2::Result result) {
                // This callback triggers when the mode completes
                switchToState(State::CustomYaw, result);
            });
            break;
        case State::CustomYaw:
            scheduleMode(_second_mode.id(), [this](px4_ros2::Result result) {
                // This callback triggers when the mode completes
                switchToState(State::Land, result);
            });
            break;
        case State::Land:
            land([this](px4_ros2::Result result) {
                switchToState(State::WaitUntilDisarmed, result);
            });
            break;
        case State::WaitUntilDisarmed:
            waitUntilDisarmed([this](px4_ros2::Result result) {
                RCLCPP_INFO(_node.get_logger(), "All states complete (%s)", resultToString(result));
            });
            break;
    }
}

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node_with_mode = std::make_shared<CustomModeWithExecutor>(kNodeName, kEnableDebugOutput);
    rclcpp::spin(node_with_mode);
    rclcpp::shutdown();
    return 0;
}
// ============================================================================
// ALTERNATIVE VERSION - For exercises
// ============================================================================
// #include "CustomModeExecutor.hpp"

// using CustomModeWithExecutor = px4_ros2::NodeWithModeExecutor<CustomModeExecutor, CustomWaypoints, CustomYaw, CustomAltitude>;

// static const std::string kNodeName = "CustomModeDemo";
// static const bool kEnableDebugOutput = true;

// CustomModeExecutor::CustomModeExecutor(rclcpp::Node &node, px4_ros2::ModeBase &owned_mode, px4_ros2::ModeBase &second_mode, px4_ros2::ModeBase &third_mode)
//     : ModeExecutorBase(node, Settings{}, owned_mode), _node(node), _second_mode(second_mode), _third_mode(third_mode) {}

// void CustomModeExecutor::onActivate() {
//     RCLCPP_INFO(_node.get_logger(), "CustomModeExecutor activated");
//     switchToState(State::Takeoff, px4_ros2::Result::Success);
// }

// void CustomModeExecutor::onDeactivate(DeactivateReason reason) {
//     const char *reason_str = (reason == DeactivateReason::FailsafeActivated)
//                                  ? "failsafe activated"
//                                  : "other reason";
//     RCLCPP_INFO(_node.get_logger(), "CustomModeExecutor deactivated: %s", reason_str);
// }

// void CustomModeExecutor::switchToState(State state, px4_ros2::Result previous_result) {
//     _state = state;
//     if (previous_result != px4_ros2::Result::Success) {
//         RCLCPP_WARN(_node.get_logger(),
//                     "Switching to state %d due to previous result: %d",
//                     static_cast<int>(state), static_cast<int>(previous_result));
//     }

//     RCLCPP_INFO(_node.get_logger(), "Switched to state: %d", static_cast<int>(state));

//     // Handle state-specific logic here
//     switch (state) {
//         case State::Takeoff:
//             RCLCPP_INFO(_node.get_logger(), "Initiating takeoff...");
//             takeoff(
//                 [this](px4_ros2::Result result) {
//                     switchToState(State::CustomWaypoints, result);
//                 },
//                 2.0f);
//             break;
//         case State::CustomWaypoints:
//             scheduleMode(ownedMode().id(), [this](px4_ros2::Result result) {
//                 // This callback triggers when the mode completes
//                 switchToState(State::CustomYaw, result);
//             });
//             break;
//         case State::CustomYaw:
//             scheduleMode(_second_mode.id(), [this](px4_ros2::Result result) {
//                 // This callback triggers when the mode completes
//                 switchToState(State::ChangeAltitude, result);
//             });
//             break;
//         case State::ChangeAltitude:
//             scheduleMode(_third_mode.id(), [this](px4_ros2::Result result) {
//                 // This callback triggers when the mode completes
//                 switchToState(State::Land, result);
//             });
//             break;
//         case State::Land:
//             land([this](px4_ros2::Result result) {
//                 switchToState(State::WaitUntilDisarmed, result);
//             });
//             break;
//         case State::WaitUntilDisarmed:
//             waitUntilDisarmed([this](px4_ros2::Result result) {
//                 RCLCPP_INFO(_node.get_logger(), "All states complete (%s)", resultToString(result));
//             });
//             break;
//     }
// }

// int main(int argc, char *argv[]) {
//     rclcpp::init(argc, argv);
//     auto node_with_mode = std::make_shared<CustomModeWithExecutor>(kNodeName, kEnableDebugOutput);
//     rclcpp::spin(node_with_mode);
//     rclcpp::shutdown();
//     return 0;
// }
