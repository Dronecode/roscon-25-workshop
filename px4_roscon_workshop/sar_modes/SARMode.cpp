#include "SARMode.hpp"

#include <px4_ros2/components/node_with_mode.hpp>

static const std::string kNodeName = "sar_mode";
static const bool kEnableDebugOutput = true;

// BaseSARMode
BaseSARMode::BaseSARMode(rclcpp::Node& node, const std::string& mode_name)
	: px4_ros2::ModeBase(node, mode_name), _node(node), _mode_name(mode_name)
{
    _trajectory_setpoint = std::make_shared<px4_ros2::TrajectorySetpointType>(*this);

    _target_sub = _node.create_subscription<geometry_msgs::msg::PoseStamped>(
		    "/rover/pose",
		    rclcpp::QoS(5).best_effort(),
		    [this](const geometry_msgs::msg::PoseStamped::SharedPtr msg) {
		        _target_pos.x() = static_cast<float>(msg->pose.position.y);   // North = ENU Y
		        _target_pos.y() = static_cast<float>(msg->pose.position.x);   // East  = ENU X
		        _target_pos.z() = -static_cast<float>(msg->pose.position.z);  // Down  = ENU -Z

		        Eigen::Quaternionf q_enu(
		            static_cast<float>(msg->pose.orientation.w),
		            static_cast<float>(msg->pose.orientation.x),
		            static_cast<float>(msg->pose.orientation.y),
		            static_cast<float>(msg->pose.orientation.z));
		        float yaw_enu = px4_ros2::quaternionToYaw(q_enu);
		        _target_yaw = px4_ros2::wrapPi(static_cast<float>(M_PI / 2.0) - yaw_enu);

		        _has_target = true;
		    });
}


void BaseSARMode::onActivate() {
    _elapsed_time = 0.0f;
    RCLCPP_INFO(_node.get_logger(), "[SAR Mode] Activated: %s", _mode_name.c_str());
}

void BaseSARMode::onDeactivate() {
    RCLCPP_INFO(_node.get_logger(), "[SAR Mode] Deactivated: %s", _mode_name.c_str());
}

// SARVSweepMode
SARVSweepMode::SARVSweepMode(rclcpp::Node& node, int drone_id, int total_drones)
        : BaseSARMode(node, "SAR (V-Sweep)"), _drone_id(drone_id), _total_drones(total_drones) {}

void SARVSweepMode::updateSetpoint(float dt) {
    (void)dt;
    if (!_has_target) return;

    float side_offset = (_drone_id - (_total_drones - 1) / 2.0f) * 6.0f;
    float forward_offset = 8.0f - std::abs(_drone_id - (_total_drones - 1) / 2.0f) * 4.0f;

    // Rotate the (forward, side) wedge offset from the rover's body frame into NED
    // using its heading, so the V stays pointed the way the rover is facing.
    float cos_yaw = std::cos(_target_yaw);
    float sin_yaw = std::sin(_target_yaw);
    float offset_north = forward_offset * cos_yaw - side_offset * sin_yaw;
    float offset_east = forward_offset * sin_yaw + side_offset * cos_yaw;

    Eigen::Vector3f offset(offset_north, offset_east, -3.0f);

    px4_ros2::TrajectorySetpoint setpoint;
    setpoint.withPosition(_target_pos + offset)
            .withYaw(_target_yaw);
    _trajectory_setpoint->update(setpoint);
}


// SAROrbitalMode
SAROrbitalMode::SAROrbitalMode(rclcpp::Node& node, int drone_id, int total_drones)
	: BaseSARMode(node, "SAR (Orbital)"), _drone_id(drone_id),
	  _total_drones(total_drones > 0 ? total_drones : 1) {}

void SAROrbitalMode::updateSetpoint(float dt) {
    if (!_has_target) return;

    _elapsed_time += dt;

    float current_radius, current_omega;
    {
        std::lock_guard<std::mutex> lock(_param_mutex);
        current_radius = _radius;
        current_omega = _omega;
    }

    float phase_offset = static_cast<float>(_drone_id) * (2.0f * M_PI / static_cast<float>(_total_drones));
    float angle = current_omega * _elapsed_time + phase_offset;
    float altitude_layer = -3.0f - (0.3f * static_cast<float>(_drone_id));

    Eigen::Vector3f offset(
        current_radius * std::cos(angle),
        current_radius * std::sin(angle),
        altitude_layer
    );

    px4_ros2::TrajectorySetpoint setpoint;
    setpoint.withPosition(_target_pos + offset)
            .withYaw(px4_ros2::wrapPi(angle + static_cast<float>(M_PI)));
    _trajectory_setpoint->update(setpoint);
}

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<rclcpp::Node>(kNodeName);

    if (kEnableDebugOutput) {
        rcutils_logging_set_logger_level(node->get_logger().get_name(), RCUTILS_LOG_SEVERITY_DEBUG);
    }

    const int drone_id = node->declare_parameter<int>("drone_id", 0);
    const int total_drones = node->declare_parameter<int>("total_drones", 3);

    auto vsweep_mode = std::make_shared<SARVSweepMode>(*node, drone_id, total_drones);
    auto orbital_mode = std::make_shared<SAROrbitalMode>(*node, drone_id, total_drones);

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
