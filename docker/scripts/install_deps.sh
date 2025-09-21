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
    bc \
    dmidecode \
    libboost-all-dev \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    libeigen3-dev \
    libgstreamer-plugins-base1.0-dev \
    libimage-exiftool-perl \
    libxml2-utils \
    pkg-config \
    protobuf-compiler \
    wget \
	libxcb-xinerama0 \
	libxkbcommon-x11-0 \
	libxcb-cursor-dev \
    ros-humble-actuator-msgs \
    ros-humble-gps-msgs \
    ros-humble-vision-msgs \
    libgflags-dev \
    python3-rospkg 

rm -rf /var/lib/apt/lists/*
apt-get clean