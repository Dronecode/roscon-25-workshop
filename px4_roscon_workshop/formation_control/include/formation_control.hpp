/****************************************************************************
 * Copyright (c) 2023 PX4 Development Team.
 * SPDX-License-Identifier: BSD-3-Clause
 ****************************************************************************/
#pragma once

#include <Eigen/Core>
#include <algorithm>
#include <GeographicLib/Geocentric.hpp>
#include <px4_ros2/components/mode.hpp>
#include <px4_ros2/odometry/local_position.hpp>
#include <px4_ros2/utils/geometry.hpp>
#include <px4_ros2/utils/frame_conversion.hpp>
#include <px4_ros2/control/setpoint_types/experimental/trajectory.hpp>
#include <rclcpp/rclcpp.hpp>
#include "tf2_ros/transform_broadcaster.hpp"
#include "tf2_ros/transform_listener.hpp"
#include "tf2_ros/buffer.hpp"

static const std::string kName = "Formation";

using namespace px4_ros2::literals;  // NOLINT

class FormationControlMode : public px4_ros2::ModeBase {
 public:
  explicit FormationControlMode(rclcpp::Node& node, const std::string& topic_namespace_prefix = "")
	: ModeBase(node, Settings{kName}, topic_namespace_prefix),
        _tf_prefix(node.get_parameter("tf_prefix").as_string()),
				_neighbor_distances(node.get_parameter("neighbor_distances").as_double_array()),
				_neighbor_prefixes(node.get_parameter("neighbor_prefixes").as_string_array()),
				_gain(node.get_parameter("gain").as_double())
  {
		for (const auto& prefix : _neighbor_prefixes) {
			_neighbor_base_link_frames.push_back(prefix + "base_link");
		}
    _tf_broadcaster =
      std::make_unique<tf2_ros::TransformBroadcaster>(node);
    _tf_buffer =
      std::make_unique<tf2_ros::Buffer>(node.get_clock());
    _tf_listener =
      std::make_shared<tf2_ros::TransformListener>(*_tf_buffer);
		_trajectory_setpoint = std::make_shared<px4_ros2::TrajectorySetpointType>(*this);
    _vehicle_local_position = std::make_shared<px4_ros2::OdometryLocalPosition>(*this);
    _vehicle_local_position->onUpdate([this](const px4_msgs::msg::VehicleLocalPosition& msg) {
			if (msg.xy_global && msg.z_global) {
				_has_global_position = true;
				if (msg.ref_timestamp != _last_global_ref_timestamp) {
					_last_global_ref_timestamp = msg.ref_timestamp;
					double x,y,z;
					_geocentric.Forward(msg.ref_lat, msg.ref_lon, msg.ref_alt, x, y, z);
					RCLCPP_INFO(this->node().get_logger(), "Global position reference updated: lat=%f, lon=%f, alt=%f",
											msg.ref_lat, msg.ref_lon, msg.ref_alt);
					RCLCPP_INFO(this->node().get_logger(), "ECEF position reference updated: x=%f, y=%f, z=%f",
											x, y, z);
					_ekf_origin.header.frame_id = "earth";
					_ekf_origin.child_frame_id = _tf_prefix + "map";
					_ekf_origin.transform.translation.x = x;
					_ekf_origin.transform.translation.y = y;
					_ekf_origin.transform.translation.z = z;
					double cos_lat = std::cos(msg.ref_lat * M_PI / 180.0);
					double sin_lat = std::sin(msg.ref_lat * M_PI / 180.0);
					double cos_lon = std::cos(msg.ref_lon * M_PI / 180.0);
					double sin_lon = std::sin(msg.ref_lon * M_PI / 180.0);
					Eigen::Matrix3d R;
					R(0,0) = -sin_lon; R(1,0) = cos_lon; R(2,0) = 0.0;
					R(0,1) = -sin_lat * cos_lon; R(1,1) = -sin_lat * sin_lon; R(2,1) = cos_lat;
					R(0,2) = cos_lat * cos_lon; R(1,2) = cos_lat * sin_lon; R(2,2) = sin_lat;
					Eigen::Quaterniond q(R);
					_ekf_origin.transform.rotation.x = q.x();
					_ekf_origin.transform.rotation.y = q.y();
					_ekf_origin.transform.rotation.z = q.z();
					_ekf_origin.transform.rotation.w = q.w();
				}
				_ekf_origin.header.stamp = this->node().get_clock()->now();
				_tf_broadcaster->sendTransform(_ekf_origin);
			} else {
				_has_global_position = false;
			}
			if (msg.xy_valid && msg.z_valid) {
				const Eigen::Vector3f pos_ned = _vehicle_local_position->positionNed();
				const Eigen::Vector3f pos_enu = px4_ros2::positionNedToEnu(pos_ned);
				geometry_msgs::msg::TransformStamped tf_msg;
				tf_msg.header.stamp = this->node().get_clock()->now();
				tf_msg.header.frame_id = _tf_prefix + "map";
				tf_msg.child_frame_id = _tf_prefix + "base_link";
				tf_msg.transform.translation.x = pos_enu(0);
				tf_msg.transform.translation.y = pos_enu(1);
				tf_msg.transform.translation.z = pos_enu(2);
				tf_msg.transform.rotation.x = 0.0;
				tf_msg.transform.rotation.y = 0.0;
				tf_msg.transform.rotation.z = 0.0;
				tf_msg.transform.rotation.w = 1.0;
				_tf_broadcaster->sendTransform(tf_msg);
			}
    });
		RCLCPP_INFO(this->node().get_logger(), "FormationControlMode initialized with %zu neighbors, gain=%f",
								_neighbor_base_link_frames.size(), _gain);
  }

  void onActivate() override
  {

  }

  void updateSetpoint(float dt_s) override
  {
		Eigen::Vector2f velocity_en{0.0, 0.0};
		for (const auto& toFrameRel : _neighbor_base_link_frames) {
			geometry_msgs::msg::TransformStamped t;
			try {
				t = _tf_buffer->lookupTransform(
					toFrameRel, _tf_prefix + "base_link",
					tf2::TimePointZero);
				const Eigen::Vector2f relative_en{t.transform.translation.x, t.transform.translation.y};
				const float distance = relative_en.norm();
				const Eigen::Vector2f direction = relative_en.normalized();
				const float distance_error = distance - _neighbor_distances[&toFrameRel - &_neighbor_base_link_frames[0]];
				const Eigen::Vector2f individual_control = - distance_error * direction * _gain;
				velocity_en.x() += individual_control.x();
				velocity_en.y() += individual_control.y();
			} catch (const tf2::TransformException & ex) {
				// RCLCPP_INFO(
				// 	this->get_logger(), "Could not transform %s to %s: %s",
				// 	toFrameRel.c_str(), fromFrameRel.c_str(), ex.what());
			}

		}
		const px4_ros2::TrajectorySetpoint setpoint = px4_ros2::TrajectorySetpoint()
			.withVelocityX(velocity_en.y())
			.withVelocityY(velocity_en.x())
			.withPositionZ(-2.0f);
    _trajectory_setpoint->update(setpoint);
  }

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
