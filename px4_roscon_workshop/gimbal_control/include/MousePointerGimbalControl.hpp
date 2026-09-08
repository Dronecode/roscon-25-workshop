#pragma once

#include <geometry_msgs/msg/point_stamped.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/camera_info.hpp>

class MousePointerGimbalControlNode : public rclcpp::Node {
 public:
  MousePointerGimbalControlNode();

 private:
  void loadParameters();
  void pointCallback(const geometry_msgs::msg::PointStamped::SharedPtr msg);
  void cameraInfoCallback(const sensor_msgs::msg::CameraInfo::SharedPtr msg);
  void controlLoop();

  rclcpp::Subscription<geometry_msgs::msg::PointStamped>::SharedPtr _point_sub;
  rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr _camera_info_sub;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr _cmd_vel_pub;
  rclcpp::TimerBase::SharedPtr _control_timer;

  geometry_msgs::msg::PointStamped::SharedPtr _last_point;
  rclcpp::Time _last_point_time;

  uint32_t _image_width{0};
  uint32_t _image_height{0};

  // Parameters
  double _param_max_pan_rate{};     // deg/s at the edge of the image
  double _param_max_tilt_rate{};    // deg/s at the edge of the image
  double _param_deadzone{};         // normalized radius around the image center with no rate
  double _param_pointer_timeout{};  // seconds before a stale pointer is treated as "not tracking"
  bool _param_invert_tilt{};        // image y grows downward, so tilt is inverted by default
};
