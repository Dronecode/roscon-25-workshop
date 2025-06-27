########################################
##         CREATE BASE IMAGE          ##
########################################
FROM ros:humble-perception AS base

SHELL ["/bin/bash", "-o", "pipefail", "-o", "errexit", "-c"]
ENV DEBIAN_FRONTEND=noninteractive
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y --no-install-recommends \
        curl \
        lsb-release \
        gnupg && \
    curl https://packages.osrfoundation.org/gazebo.gpg --output /usr/share/keyrings/pkgs-osrf-archive-keyring.gpg && \
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/pkgs-osrf-archive-keyring.gpg] http://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/gazebo-stable.list > /dev/null && \
    apt-get update && \
    apt-get install -y gz-harmonic

ARG USER=ubuntu
# TO-DO fix permission
RUN groupadd -g 1000 ${USER} && \
	useradd -m -u 1000 -g ${USER} ${USER}

RUN apt-get -y --quiet --no-install-recommends install \
    bc \
    dmidecode \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    libeigen3-dev \
    libgstreamer-plugins-base1.0-dev \
    libimage-exiftool-perl \
    libopencv-dev \
    libxml2-utils \
    pkg-config \
    protobuf-compiler

RUN apt-get install -y --no-install-recommends \
	wget \
# for QGC
	fuse3 \
	libxcb-xinerama0 \
	libxkbcommon-x11-0 \
	libxcb-cursor-dev \
# for px4_sitl Appimage
	libfuse2

USER ${USER}

WORKDIR /home/${USER}

RUN git clone --depth 1 https://github.com/PX4/PX4-gazebo-models.git

RUN wget https://github.com/mavlink/qgroundcontrol/releases/download/v5.0.4/QGroundControl-x86_64.AppImage && \
	chmod +x QGroundControl-x86_64.AppImage