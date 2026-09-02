#pragma once

// PX4 Interface Library
#include <px4_ros2/components/mode.hpp>
#include <px4_ros2/control/setpoint_types/experimental/trajectory.hpp>
#include <px4_ros2/control/setpoint_types/fixedwing/lateral_longitudinal.hpp>
#include <px4_ros2/control/vtol.hpp>

// ROS 2 Core
#include <rclcpp/rclcpp.hpp>

// C++ Std
#include <Eigen/Eigen>

class VtolCustomMode : public px4_ros2::ModeBase {
 public:
  explicit VtolCustomMode(rclcpp::Node& node);

  // See ModeBase
  void onActivate() override;
  void onDeactivate() override;
  void updateSetpoint([[maybe_unused]] float dt_s) override;

 private:
  void loadParameters();

  // Handles one phase's worth of setpoint publishing, returns true once
  // that phase's exit condition is met.
  void updateDuringTransition();
  void updateDuringCruise();

  rclcpp::Node& _node;

  // px4_ros2_cpp
  std::shared_ptr<px4_ros2::VTOL> _vtol;
  std::shared_ptr<px4_ros2::FwLateralLongitudinalSetpointType> _fw_setpoint;
  std::shared_ptr<px4_ros2::TrajectorySetpointType> _trajectory_setpoint;

  // Internal phases. This mode owns the whole fixed-wing excursion,
  // including both VTOL transitions, so the executor only has to
  // schedule it once between the inbuilt Takeoff and Land modes.
  enum class Phase {
    TransitionToFixedWing,
    Cruise,
    TransitionToMulticopter,
  };
  Phase _phase;

  rclcpp::Time _cruise_start_time;

  // Parameters
  float _param_cruise_altitude_amsl = {};
  float _param_cruise_course_deg = {};
  float _param_cruise_airspeed = {};
  float _param_cruise_duration_s = {};
};
