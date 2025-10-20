#!/usr/bin/env bash
set -eo pipefail

MICRO_XRCE_DDS_AGENT_VERSION=$1
PX4_MSGS_VERSION=$2
PX4_ROS2_INTERFACE_LIB_VERSION=$3
PX4_ROS_COM_VERSION=$4

mkdir -p /root/px4_ros_ws/src && cd /root/px4_ros_ws/src && \
git clone --depth 1 -b ${MICRO_XRCE_DDS_AGENT_VERSION} https://github.com/eProsima/Micro-XRCE-DDS-Agent.git && \
git clone --depth 1 -b ${PX4_MSGS_VERSION} https://github.com/PX4/px4_msgs.git && \
git clone --depth 1 -b ${PX4_ROS2_INTERFACE_LIB_VERSION} https://github.com/Auterion/px4-ros2-interface-lib.git && \
git clone --depth 1 -b ${PX4_ROS_COM_VERSION} https://github.com/PX4/px4_ros_com.git && \
git clone --depth 1 -b humble https://github.com/ros-perception/vision_opencv.git && \
git clone --depth 1 -b humble https://github.com/ros-perception/image_common.git && \
git clone --depth 1 -b humble https://github.com/ros-perception/image_transport_plugins.git && \
git clone --depth 1 -b humble https://github.com/gazebosim/ros_gz.git && \
rm -rf ros_gz/ros_ign* \
    ros_gz/ros_gz_sim_demos \
    image_transport_plugins/compressed_depth_image_transport \
    image_transport_plugins/theora_image_transport \
    image_transport_plugins/zstd_image_transport \
    image_transport_plugins/image_transport_plugins
cd .. && source /opt/ros/humble/setup.bash
GZ_VERSION=harmonic colcon build