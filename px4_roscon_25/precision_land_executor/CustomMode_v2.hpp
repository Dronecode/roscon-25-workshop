// ============================================================================
// ORIGINAL VERSION - CustomMode_v2.hpp
// ============================================================================
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
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/bool.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/quaternion.hpp>

// C++ Std
#include <cmath>  // for M_PI
#include <Eigen/Eigen>
#include <chrono>
#include <vector>

class CustomWaypoints : public px4_ros2::ModeBase {
public:
    explicit CustomWaypoints(rclcpp::Node &node);

    // See ModeBase
    void onActivate() override;
    void onDeactivate() override;
    void updateSetpoint([[maybe_unused]] float dt_s) override;

private:
    void loadParameters();
    // ROS 2
    rclcpp::Node &_node;


    // px4_ros2_cpp
    std::shared_ptr<px4_ros2::TrajectorySetpointType> _trajectory_setpoint;
    std::shared_ptr<px4_ros2::OdometryLocalPosition> _local_position;

    std::vector<Eigen::Vector3f> _trajectory_waypoints; // Vector to hold waypoints
    size_t _current_waypoint_index; // Index of the current waypoint
};
// PrecisionLand
class PrecisionLand : public px4_ros2::ModeBase
{
public:
	explicit PrecisionLand(rclcpp::Node& node);

	void targetPoseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr msg);
	void vehicleLandDetectedCallback(const px4_msgs::msg::VehicleLandDetected::SharedPtr msg);

	// See ModeBasep
	void onActivate() override;
	void onDeactivate() override;
	void updateSetpoint(float dt_s) override;

private:
	struct ArucoTag {
		// Initialize position with NaN values directly in the struct
		Eigen::Vector3d position = Eigen::Vector3d::Constant(std::numeric_limits<double>::quiet_NaN());
		Eigen::Quaterniond orientation;
		rclcpp::Time timestamp;

		bool valid() { return timestamp.nanoseconds() > 0; };
	};

	void loadParameters();

	ArucoTag getTagWorld(const ArucoTag& tag);

	Eigen::Vector2f calculateVelocitySetpointXY();
	bool checkTargetTimeout();
	bool positionReached(const Eigen::Vector3f& target) const;

	enum class State {
		Idle,
		Search, 	// Searches for target using a search pattern
		Approach, 	// Positioning over landing target while maintaining altitude
		Descend, 	// Stay over landing target while descending
		Finished
	};

	void switchToState(State state);
	std::string stateName(State state);

	// ros2
	rclcpp::Node& _node;
	rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr _target_pose_sub;
	rclcpp::Subscription<px4_msgs::msg::VehicleLandDetected>::SharedPtr _vehicle_land_detected_sub;

	// px4_ros2_cpp
	std::shared_ptr<px4_ros2::OdometryLocalPosition> _vehicle_local_position;
	std::shared_ptr<px4_ros2::OdometryAttitude> _vehicle_attitude;
	std::shared_ptr<px4_ros2::TrajectorySetpointType> _trajectory_setpoint;

	// Data
	State _state = State::Search;
	bool _search_started = false;

	ArucoTag _tag;
	float _approach_altitude = {};

	// Land detection
	bool _land_detected = false;
	bool _target_lost_prev = true;

	// Waypoints for Search pattern
	std::vector<Eigen::Vector3f> _search_waypoints;
	// Search pattern generation
	void generateSearchWaypoints();
	// Search pattern index
	int _search_waypoint_index = 0;

	// Parameters
	float _param_descent_vel = {};
	float _param_vel_p_gain = {};
	float _param_vel_i_gain = {};
	float _param_max_velocity = {};
	float _param_target_timeout = {};
	float _param_delta_position = {};
	float _param_delta_velocity = {};

	float _vel_x_integral {};
	float _vel_y_integral {};
};
