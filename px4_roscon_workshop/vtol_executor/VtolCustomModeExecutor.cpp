#include "VtolCustomModeExecutor.hpp"

using VtolModeWithExecutor = px4_ros2::NodeWithModeExecutor<VtolCustomModeExecutor, VtolCustomMode>;

static const std::string kNodeName = "vtol_executor";
static const bool kEnableDebugOutput = true;

VtolCustomModeExecutor::VtolCustomModeExecutor(px4_ros2::ModeBase &owned_mode)
    : ModeExecutorBase(Settings{}, owned_mode)
{
    node().declare_parameter<float>("takeoff_altitude_amsl", 50.0);
    node().get_parameter("takeoff_altitude_amsl", _param_takeoff_altitude_amsl);
}

void VtolCustomModeExecutor::onActivate() {
    RCLCPP_INFO(node().get_logger(), "VtolCustomModeExecutor activated");
    switchToState(State::Takeoff, px4_ros2::Result::Success);
}

void VtolCustomModeExecutor::onDeactivate(DeactivateReason reason) {
    const char *reason_str = (reason == DeactivateReason::FailsafeActivated)
                                 ? "failsafe activated"
                                 : "other reason";
    RCLCPP_INFO(node().get_logger(), "VtolCustomModeExecutor deactivated: %s", reason_str);
}

void VtolCustomModeExecutor::switchToState(State state, px4_ros2::Result previous_result) {
    _state = state;
    if (previous_result != px4_ros2::Result::Success) {
        RCLCPP_WARN(node().get_logger(),
                    "Switching to state %d due to previous result: %d",
                    static_cast<int>(state), static_cast<int>(previous_result));
    }

    RCLCPP_INFO(node().get_logger(), "Switched to state: %d", static_cast<int>(state));

    switch (state) {
        case State::Takeoff:
            RCLCPP_INFO(node().get_logger(), "Initiating takeoff (multicopter)...");
            takeoff(
                [this](px4_ros2::Result result) {
                    switchToState(State::Cruise, result);
                },
                _param_takeoff_altitude_amsl);
            break;

        case State::Cruise:
            // VtolCustomMode handles the transition to fixed-wing, the
            // cruise leg, and the transition back to multicopter. By the
            // time it completes we're multicopter again but potentially far
            // from home, so RTL (not a plain Land) to actually get back.
            scheduleMode(ownedMode().id(), [this](px4_ros2::Result result) {
                switchToState(State::ReturnToLaunch, result);
            });
            break;

        case State::ReturnToLaunch:
            RCLCPP_INFO(node().get_logger(), "Returning to launch...");
            rtl([this](px4_ros2::Result result) {
                switchToState(State::WaitUntilDisarmed, result);
            });
            break;

        case State::WaitUntilDisarmed:
            waitUntilDisarmed([this](px4_ros2::Result result) {
                RCLCPP_INFO(node().get_logger(), "All states complete (%s)", resultToString(result));
            });
            break;
    }
}

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node_with_mode = std::make_shared<VtolModeWithExecutor>(kNodeName, kEnableDebugOutput);
    rclcpp::spin(node_with_mode);
    rclcpp::shutdown();
    return 0;
}
