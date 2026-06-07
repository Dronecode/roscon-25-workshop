#!/usr/bin/env bash
set -eo pipefail

PX4_MSGS_VERSION=$1
PX4_ROS2_INTERFACE_LIB_VERSION=$2

mkdir -p /root/px4_ros_ws/src && cd /root/px4_ros_ws/src && \
git clone --depth 1 -b ${PX4_MSGS_VERSION} https://github.com/PX4/px4_msgs.git && \
git clone --depth 1 -b ${PX4_ROS2_INTERFACE_LIB_VERSION} https://github.com/Auterion/px4-ros2-interface-lib.git && \
cd .. && source /opt/ros/$ROS_DISTRO/setup.bash
colcon build