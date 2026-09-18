/****************************************************************************
 * Copyright (c) 2023 PX4 Development Team.
 * SPDX-License-Identifier: BSD-3-Clause
 ****************************************************************************/
#pragma once

#include <Eigen/Core>
#include <GeographicLib/Geocentric.hpp>
#include <algorithm>
#include <px4_ros2/components/mode.hpp>
#include <px4_ros2/control/setpoint_types/experimental/trajectory.hpp>
#include <px4_ros2/odometry/local_position.hpp>
#include <px4_ros2/utils/frame_conversion.hpp>
#include <px4_ros2/utils/geometry.hpp>
#include <rclcpp/rclcpp.hpp>

#include "tf2_ros/buffer.hpp"
#include "tf2_ros/transform_broadcaster.hpp"
#include "tf2_ros/transform_listener.hpp"

using namespace px4_ros2::literals;  // NOLINT

class FormationControlMode : public px4_ros2::ModeBase {
 public:
  explicit FormationControlMode(rclcpp::Node& node, const std::string& topic_namespace_prefix = "");

  void onActivate() override;
  void updateSetpoint(float dt_s) override;

 private:
  GeographicLib::Geocentric _geocentric{GeographicLib::Geocentric::WGS84()};
  std::shared_ptr<px4_ros2::OdometryLocalPosition> _vehicle_local_position;
  bool _has_global_position{false};
  uint64_t _last_global_ref_timestamp{0};
  std::unique_ptr<tf2_ros::TransformBroadcaster> _tf_broadcaster;
  geometry_msgs::msg::TransformStamped _ekf_origin;
  const std::string _tf_prefix;
  const std::vector<double> _neighbor_distances;
  const std::vector<std::string> _neighbor_prefixes;
  const double _gain;
  std::vector<std::string> _neighbor_base_link_frames;
  std::shared_ptr<tf2_ros::TransformListener> _tf_listener{nullptr};
  std::unique_ptr<tf2_ros::Buffer> _tf_buffer;
  std::shared_ptr<px4_ros2::TrajectorySetpointType> _trajectory_setpoint;
};
