/****************************************************************************
 * Copyright (c) 2026 PX4 Development Team.
 * SPDX-License-Identifier: BSD-3-Clause
 ****************************************************************************/
#pragma once

#include <Eigen/Eigen>
#include <algorithm>
#include <cmath>
#include <px4_ros2/components/mode.hpp>
#include <px4_ros2/control/setpoint_types/direct_actuators.hpp>
#include <rclcpp/rclcpp.hpp>

static const std::string kModeName = "Motor Ramp Test";
static constexpr float kMaxThrottleLimit = 0.50f;  ///< hard cap: 50% thrust

/**
 * @brief Test mode that ramps motor 1 (index 0) up and down between 0% and a
 *        configurable max throttle using direct actuator control. Each ramp
 *        segment (up or down) takes "ramp_period_s" seconds. The max
 *        throttle, set via the "max_throttle" parameter, is hard-limited to
 *        kMaxThrottleLimit for safety.
 */
class MotorRampMode : public px4_ros2::ModeBase {
 public:
  explicit MotorRampMode(rclcpp::Node& node)
      : ModeBase(node, Settings{kModeName}.activateEvenWhileDisarmed(true).preventArming(false))
  {
    _direct_actuators_setpoint = std::make_shared<px4_ros2::DirectActuatorsSetpointType>(*this);

    node.declare_parameter<double>("ramp_period_s", 5.0);
    node.declare_parameter<double>("max_throttle", 0.10);

    double ramp_period_s{5.0};
    double max_throttle{0.10};
    node.get_parameter("ramp_period_s", ramp_period_s);
    node.get_parameter("max_throttle", max_throttle);

    _ramp_period_s = static_cast<float>(ramp_period_s);
    _max_throttle = std::min(static_cast<float>(max_throttle), kMaxThrottleLimit);
  }

  void onActivate() override { resetRamp(); }

  void updateSetpoint(float dt_s) override
  {
    // Hold the ramp at 0% until armed, so it always restarts from 0% at
    // whichever comes later: switching into this mode, or arming.
    if (!isArmed()) {
      resetRamp();
    } else {
      if (!_was_armed) {
        resetRamp();
      }
      _elapsed_s += dt_s;
      if (_elapsed_s >= _ramp_period_s) {
        _elapsed_s -= _ramp_period_s;
        _ramping_up = !_ramping_up;
      }
    }
    _was_armed = isArmed();

    const float fraction = _elapsed_s / _ramp_period_s;
    const float motor1_thrust = _max_throttle * (_ramping_up ? fraction : (1.f - fraction));

    Eigen::Matrix<float, px4_ros2::DirectActuatorsSetpointType::kMaxNumMotors, 1> motor_commands =
        Eigen::Matrix<float, px4_ros2::DirectActuatorsSetpointType::kMaxNumMotors, 1>::Constant(
            NAN);
    motor_commands(0) = motor1_thrust;  // first motor

    _direct_actuators_setpoint->updateMotors(motor_commands);
  }

 private:
  void resetRamp()
  {
    _elapsed_s = 0.f;
    _ramping_up = true;
  }

  std::shared_ptr<px4_ros2::DirectActuatorsSetpointType> _direct_actuators_setpoint;
  float _ramp_period_s{5.f};
  float _max_throttle{0.10f};
  float _elapsed_s{0.f};
  bool _ramping_up{true};
  bool _was_armed{false};
};
