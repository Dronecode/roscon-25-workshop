#pragma once

#include <rclcpp/rclcpp.hpp>
#include "tf2_ros/transform_broadcaster.h"
#include "tf2_ros/static_transform_broadcaster.h"
#include "px4_msgs/msg/vehicle_odometry.hpp"

class Px4TfPublisherNode : public rclcpp::Node {
public:
    Px4TfPublisherNode();
private:
    std::string px4_tf_prefix_;
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    std::unique_ptr<tf2_ros::StaticTransformBroadcaster> tf_static_broadcaster_;
    rclcpp::Subscription<px4_msgs::msg::VehicleOdometry>::SharedPtr odom_sub_;
    void make_static_transforms();
    void handle_odometry(const px4_msgs::msg::VehicleOdometry::SharedPtr msg);
};