#include "TeleopExecutor.hpp"

using TeleopNodeWithExecutor = px4_ros2::NodeWithModeExecutor<TeleopExecutor, Teleop>;

static const std::string kNodeName = "teleop_node";
static const bool kEnableDebugOutput = true;

TeleopExecutor::TeleopExecutor(rclcpp::Node &node, px4_ros2::ModeBase &owned_mode)
    : ModeExecutorBase(node, Settings{}, owned_mode), _node(node) {}

void TeleopExecutor::onActivate() {
    RCLCPP_INFO(_node.get_logger(), "TeleopExecutor activated");
    switchToState(State::Takeoff, px4_ros2::Result::Success);
}

void TeleopExecutor::onDeactivate(DeactivateReason reason) {
    const char *reason_str = (reason == DeactivateReason::FailsafeActivated)
                                 ? "failsafe activated"
                                 : "other reason";
    RCLCPP_INFO(_node.get_logger(), "TeleopExecutor deactivated: %s", reason_str);
}

void TeleopExecutor::switchToState(State state, px4_ros2::Result previous_result) {
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
                    switchToState(State::TeleOperation, result);
                },
                5.0f);
            break;
        case State::TeleOperation:
            scheduleMode(ownedMode().id(), [this](px4_ros2::Result result) {
                // This callback triggers when the mode completes
                if (result == px4_ros2::Result::Success) {
                    switchToState(State::Land, result);
                } else {
                    switchToState(State::RTL, result);
                }
            });
            break;
        case State::RTL:
            rtl([this](px4_ros2::Result result) {
                switchToState(State::WaitUntilDisarmed, result);
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
    auto node_with_mode = std::make_shared<TeleopNodeWithExecutor>(kNodeName, kEnableDebugOutput);
    rclcpp::spin(node_with_mode);
    rclcpp::shutdown();
    return 0;
}
