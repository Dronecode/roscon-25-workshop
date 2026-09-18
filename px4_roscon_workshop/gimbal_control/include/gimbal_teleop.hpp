#pragma once

#include <geometry_msgs/msg/twist.hpp>
#include <px4_msgs/msg/gimbal_controls.hpp>
#include <rclcpp/rclcpp.hpp>

class GimbalTeleopNode : public rclcpp::Node {
 public:
  GimbalTeleopNode();

 private:
  void loadParameters();
  void twistCallback(const geometry_msgs::msg::Twist::SharedPtr msg);
  void controlLoop();

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr _twist_sub;
  rclcpp::Publisher<px4_msgs::msg::GimbalControls>::SharedPtr _gimbal_controls_pub;
  rclcpp::TimerBase::SharedPtr _control_timer;

  double _pan_rate{0.0};   // deg/s, from the latest Twist message
  double _tilt_rate{0.0};  // deg/s, from the latest Twist message
  rclcpp::Time _last_twist_time;

  double _yaw_angle{0.0};    // deg, integrated pan angle
  double _pitch_angle{0.0};  // deg, integrated tilt angle
  rclcpp::Time _last_update_time;

  // Parameters
  double _param_pan_gain{};
  double _param_tilt_gain{};
  double _param_yaw_limit{};
  double _param_pitch_min{};
  double _param_pitch_max{};
};
