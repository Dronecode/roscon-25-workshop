#include "VtolCustomMode.hpp"

#include <cmath>  // for M_PI, NAN

static const std::string kModeName = "VtolCruise";

VtolCustomMode::VtolCustomMode(rclcpp::Node &node)
    : px4_ros2::ModeBase(node, kModeName)
    , _node(node)
{
    loadParameters();

    // Both setpoint types must be available for the duration of this mode:
    // FwLateralLongitudinalSetpointType while cruising in fixed-wing, and
    // both of them together while transitioning (see updateDuringTransition()).
    _vtol = std::make_shared<px4_ros2::VTOL>(*this);
    _fw_setpoint = std::make_shared<px4_ros2::FwLateralLongitudinalSetpointType>(*this);
    _trajectory_setpoint = std::make_shared<px4_ros2::TrajectorySetpointType>(*this);
}

void VtolCustomMode::loadParameters()
{
    _node.declare_parameter<float>("cruise_altitude_amsl", 60.0);
    _node.declare_parameter<float>("cruise_course_deg", 0.0);
    _node.declare_parameter<float>("cruise_airspeed", 15.0);
    _node.declare_parameter<float>("cruise_duration_s", 30.0);

    _node.get_parameter("cruise_altitude_amsl", _param_cruise_altitude_amsl);
    _node.get_parameter("cruise_course_deg", _param_cruise_course_deg);
    _node.get_parameter("cruise_airspeed", _param_cruise_airspeed);
    _node.get_parameter("cruise_duration_s", _param_cruise_duration_s);

    RCLCPP_INFO(_node.get_logger(), "cruise_altitude_amsl: %f", _param_cruise_altitude_amsl);
    RCLCPP_INFO(_node.get_logger(), "cruise_course_deg: %f", _param_cruise_course_deg);
    RCLCPP_INFO(_node.get_logger(), "cruise_airspeed: %f", _param_cruise_airspeed);
    RCLCPP_INFO(_node.get_logger(), "cruise_duration_s: %f", _param_cruise_duration_s);
}

void VtolCustomMode::onActivate()
{
    RCLCPP_INFO(_node.get_logger(), "VtolCruise mode activated: commanding transition to fixed-wing");
    _phase = Phase::TransitionToFixedWing;
    _vtol->toFixedwing();
}

void VtolCustomMode::onDeactivate()
{
    RCLCPP_INFO(_node.get_logger(), "VtolCruise mode deactivated");
}

void VtolCustomMode::updateDuringTransition()
{
    // While transitioning, PX4 expects both a TrajectorySetpointType (to
    // handle the multicopter side: hold altitude, follow the commanded
    // transition acceleration) and a FwLateralLongitudinalSetpointType (to
    // realign heading, altitude left NAN) streamed every cycle.
    const Eigen::Vector3f acceleration_sp = _vtol->computeAccelerationSetpointDuringTransition();
    const Eigen::Vector3f velocity_sp{NAN, NAN, 0.f};
    _trajectory_setpoint->update(velocity_sp, acceleration_sp);

    const float course_rad = _param_cruise_course_deg * static_cast<float>(M_PI) / 180.f;
    _fw_setpoint->updateWithAltitude(NAN, course_rad);
}

void VtolCustomMode::updateDuringCruise()
{
    const float course_rad = _param_cruise_course_deg * static_cast<float>(M_PI) / 180.f;
    _fw_setpoint->updateWithAltitude(_param_cruise_altitude_amsl, course_rad, _param_cruise_airspeed);
}

void VtolCustomMode::updateSetpoint([[maybe_unused]] float dt_s)
{
    switch (_phase) {
    case Phase::TransitionToFixedWing:
        updateDuringTransition();

        if (_vtol->getCurrentState() == px4_ros2::VTOL::State::FixedWing) {
            RCLCPP_INFO(_node.get_logger(), "Transition to fixed-wing complete, starting cruise");
            _phase = Phase::Cruise;
            _cruise_start_time = _node.now();
        }
        break;

    case Phase::Cruise:
        updateDuringCruise();

        if ((_node.now() - _cruise_start_time).seconds() > _param_cruise_duration_s) {
            RCLCPP_INFO(_node.get_logger(), "Cruise duration elapsed, commanding transition to multicopter");
            _phase = Phase::TransitionToMulticopter;
            _vtol->toMulticopter();
        }
        break;

    case Phase::TransitionToMulticopter:
        updateDuringTransition();

        if (_vtol->getCurrentState() == px4_ros2::VTOL::State::Multicopter) {
            RCLCPP_INFO(_node.get_logger(), "Transition to multicopter complete, completing VtolCruise mode");
            completed(px4_ros2::Result::Success);
            return;
        }
        break;
    }
}
