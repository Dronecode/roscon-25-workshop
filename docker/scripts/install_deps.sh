#!/usr/bin/env bash
set -euo pipefail

apt-get update && \
apt-get upgrade -y && \
apt-get install -y --no-install-recommends \
    curl \
    lsb-release \
    gnupg && \
curl https://packages.osrfoundation.org/gazebo.gpg --output /usr/share/keyrings/pkgs-osrf-archive-keyring.gpg && \
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/pkgs-osrf-archive-keyring.gpg] http://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/gazebo-stable.list > /dev/null && \
apt-get update && \
apt-get install -y --no-install-recommends \
    gz-harmonic \
    ros-dev-tools \
    ros-$ROS_DISTRO-ros-gz \
    ros-$ROS_DISTRO-rmw-zenoh-cpp

# rm -rf /var/lib/apt/lists/*
# apt-get clean