#include "px4_tf/px4_tf_publisher_node.hpp"
#include "tf2/LinearMath/Quaternion.h"

Px4TfPublisherNode::Px4TfPublisherNode()
    : Node("px4_tf_publisher") {
        px4_tf_prefix_ = this->declare_parameter<std::string>("px4_tf_prefix", "");
        tf_broadcaster_ =
            std::make_unique<tf2_ros::TransformBroadcaster>(*this);
        tf_static_broadcaster_ = 
            std::make_unique<tf2_ros::StaticTransformBroadcaster>(*this);
        this->make_static_transforms();
        odom_sub_ = this->create_subscription<px4_msgs::msg::VehicleOdometry>(
            "fmu/out/vehicle_odometry",
            rclcpp::SensorDataQoS(),
            [this](const px4_msgs::msg::VehicleOdometry::SharedPtr msg) {
                this->handle_odometry(msg);
            }
        );
}

void Px4TfPublisherNode::make_static_transforms() {
    std::vector<geometry_msgs::msg::TransformStamped> static_transforms;
    geometry_msgs::msg::TransformStamped t;

    // odom ENU to odom NED
    t.header.stamp = this->get_clock()->now();
    t.header.frame_id = px4_tf_prefix_ + "odom";
    t.child_frame_id = px4_tf_prefix_ + "odom_ned";
    t.transform.translation.x = 0.0;
    t.transform.translation.y = 0.0;
    t.transform.translation.z = 0.0;
    tf2::Quaternion q;
    q.setRPY(M_PI, 0, M_PI/2);
    t.transform.rotation.x = q.x();
    t.transform.rotation.y = q.y();
    t.transform.rotation.z = q.z();
    t.transform.rotation.w = q.w();
    static_transforms.push_back(t);

    // base link flu to base link frd
    t.header.frame_id = px4_tf_prefix_ + "base_link_frd";
    t.child_frame_id = px4_tf_prefix_ + "base_link";
    q.setRPY(M_PI, 0, 0);
    t.transform.rotation.x = q.x();
    t.transform.rotation.y = q.y();
    t.transform.rotation.z = q.z();
    t.transform.rotation.w = q.w();
    static_transforms.push_back(t);

    tf_static_broadcaster_->sendTransform(static_transforms);
}

void Px4TfPublisherNode::handle_odometry(
    const px4_msgs::msg::VehicleOdometry::SharedPtr msg) {
    geometry_msgs::msg::TransformStamped t;
    t.header.stamp = this->get_clock()->now();
    t.header.frame_id = px4_tf_prefix_ + "odom_ned";
    t.child_frame_id = px4_tf_prefix_ + "base_link_frd";
    t.transform.translation.x = msg->position[0];
    t.transform.translation.y = msg->position[1];
    t.transform.translation.z = msg->position[2];
    t.transform.rotation.x = msg->q[1];
    t.transform.rotation.y = msg->q[2];
    t.transform.rotation.z = msg->q[3];
    t.transform.rotation.w = msg->q[0];
    tf_broadcaster_->sendTransform(t);
}
