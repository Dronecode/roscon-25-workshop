#include "SARModeExecutor.hpp"

static const std::string kNodeName = "sar_auto_mode";
static const bool kEnableDebugOutput = true;

SARModeExecutor::SARModeExecutor(rclcpp::Node& node, px4_ros2::ModeBase& owned_mode,
                                   px4_ros2::ModeBase& vsweep_mode, px4_ros2::ModeBase& orbital_mode)
    : ModeExecutorBase(Settings{}, owned_mode), _node(node),
      _vsweep_mode(vsweep_mode), _orbital_mode(orbital_mode)
{
    _target_sub = _node.create_subscription<geometry_msgs::msg::PoseStamped>(
        "/rover/pose",
        rclcpp::QoS(5).best_effort(),
        [this](const geometry_msgs::msg::PoseStamped::SharedPtr msg) {
            targetPoseCallback(msg);
        });
}

void SARModeExecutor::targetPoseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr msg) {
    rclcpp::Time stamp(msg->header.stamp);
    float x = static_cast<float>(msg->pose.position.x);
    float y = static_cast<float>(msg->pose.position.y);

    if (_has_prev) {
        double dt = (stamp - _prev_stamp).seconds();
        if (dt > 1e-3) {
            float dx = x - _prev_x;
            float dy = y - _prev_y;
            _current_speed = std::sqrt(dx * dx + dy * dy) / static_cast<float>(dt);
        }
    } else {
        _has_prev = true;
    }

    _prev_x = x;
    _prev_y = y;
    _prev_stamp = stamp;

    evaluateAndSwitch();
}

void SARModeExecutor::evaluateAndSwitch() {
    if (!isInCharge()) return;

    px4_ros2::ModeBase* desired = nullptr;

    if (_active_mode_id == _orbital_mode.id()) {
        if (_current_speed >= kSpeedHighThreshold) desired = &_vsweep_mode;
    } else if (_active_mode_id == _vsweep_mode.id()) {
        if (_current_speed < kSpeedLowThreshold) desired = &_orbital_mode;
    } else {
        // First decision after (re)activation: no hysteresis state yet.
        desired = (_current_speed < kSpeedLowThreshold) ? &_orbital_mode : &_vsweep_mode;
    }

    if (desired == nullptr) return;  // inside the hysteresis band, or already correct

    _active_mode_id = desired->id();
    scheduleMode(desired->id(), [this](px4_ros2::Result result) {
        RCLCPP_INFO(_node.get_logger(), "SAR Auto: scheduled mode ended (%s)",
                    px4_ros2::resultToString(result));
    });
}

void SARModeExecutor::onActivate() {
    RCLCPP_INFO(_node.get_logger(), "[SAR Mode] Activated: SAR (Auto)");
    _active_mode_id = px4_ros2::ModeBase::kModeIDInvalid;  // force a fresh decision
    evaluateAndSwitch();
}

void SARModeExecutor::onDeactivate(DeactivateReason reason) {
    const char* reason_str = (reason == DeactivateReason::FailsafeActivated)
                                  ? "failsafe activated"
                                  : "other reason";
    RCLCPP_INFO(_node.get_logger(), "[SAR Mode] Deactivated: SAR (Auto) (%s)", reason_str);
}

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<rclcpp::Node>(kNodeName);

    if (kEnableDebugOutput) {
        auto ret = rcutils_logging_set_logger_level(node->get_logger().get_name(), RCUTILS_LOG_SEVERITY_DEBUG);
        if (ret != RCUTILS_RET_OK) {
            RCLCPP_ERROR(node->get_logger(), "Error setting severity: %s", rcutils_get_error_string().str);
            rcutils_reset_error();
        }
    }

    const int drone_id = node->declare_parameter<int>("drone_id", 0);
    const int total_drones = node->declare_parameter<int>("total_drones", 3);

    auto auto_mode = std::make_shared<SARAutoMode>(*node);
    auto vsweep_mode = std::make_shared<SARVSweepMode>(*node, drone_id, total_drones);
    auto orbital_mode = std::make_shared<SAROrbitalMode>(*node, drone_id, total_drones);
    auto executor = std::make_shared<SARModeExecutor>(*node, *auto_mode, *vsweep_mode, *orbital_mode);

    if (!executor->doRegister()) {
        RCLCPP_ERROR(node->get_logger(), "SAR Auto executor registration failed");
        return -1;
    }
    if (!vsweep_mode->doRegister()) {
        RCLCPP_ERROR(node->get_logger(), "SAR V-Sweep mode registration failed");
        return -1;
    }
    if (!orbital_mode->doRegister()) {
        RCLCPP_ERROR(node->get_logger(), "SAR Orbital mode registration failed");
        return -1;
    }

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
