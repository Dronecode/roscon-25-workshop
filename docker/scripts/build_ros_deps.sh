#!/usr/bin/env bash
set -eo pipefail

MICRO_XRCE_DDS_AGENT_VERSION=$1
PX4_MSGS_VERSION=$2
PX4_ROS2_INTERFACE_LIB_VERSION=$3

mkdir -p /root/px4_ros_ws/src && cd /root/px4_ros_ws/src && \
git clone --depth 1 -b ${MICRO_XRCE_DDS_AGENT_VERSION} https://github.com/eProsima/Micro-XRCE-DDS-Agent.git && \
git clone --depth 1 -b ${PX4_MSGS_VERSION} https://github.com/PX4/px4_msgs.git && \
git clone --depth 1 -b ${PX4_ROS2_INTERFACE_LIB_VERSION} https://github.com/Auterion/px4-ros2-interface-lib.git && \
cd .. && source /opt/ros/humble/setup.bash && \
colcon build