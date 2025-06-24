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
RUN groupadd -g 1001 ${USER} && \
	useradd -m -u 1001 -g ${USER} ${USER}

########################################
##          CREATE DEV PX4            ##
########################################
FROM base AS dev-px4

# base PX4 build dependencies
RUN apt-get -y --quiet --no-install-recommends install \
	astyle \
	build-essential \
	ccache \
	cmake \
	cppcheck \
	file \
	g++ \
	gcc \
	gdb \
	git \
	lcov \
	libssl-dev \
	libxml2-dev \
	libxml2-utils \
	make \
	ninja-build \
	python3 \
	python3-dev \
	python3-pip \
	python3-setuptools \
	python3-wheel \
	rsync \
	shellcheck \
	unzip \
	zip

# base PX4 build python dependencies
USER ${USER}
RUN --mount=type=bind,source=docker/requirements.txt,target=/tmp/requirements.txt python3 -m pip install -r /tmp/requirements.txt
USER root

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

USER ${USER}
WORKDIR /home/${USER}

RUN git clone --recurse-submodules -b release/1.16 https://github.com/PX4/PX4-Autopilot.git

RUN cd PX4-Autopilot && make px4_sitl

USER root
RUN apt-get install -y --no-install-recommends \
	wget \
	fuse3 \
	libxcb-xinerama0 \
	libxkbcommon-x11-0 \
	libxcb-cursor-dev
USER ${USER}
RUN cd ~ && wget https://github.com/mavlink/qgroundcontrol/releases/download/v5.0.3/QGroundControl-x86_64.AppImage && \
	chmod +x QGroundControl-x86_64.AppImage