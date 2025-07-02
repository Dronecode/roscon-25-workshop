########################################
##         CREATE BASE IMAGE          ##
########################################
FROM ros:humble-perception AS base

SHELL ["/bin/bash", "-o", "pipefail", "-o", "errexit", "-c"]
ENV DEBIAN_FRONTEND=noninteractive
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

# install dependencies
RUN --mount=type=bind,source=./docker/scripts/install_deps.sh,target=/root/scripts/install_deps.sh /root/scripts/install_deps.sh

# Add user
ENV USER=ubuntu
#TO-DO fix permission
RUN groupadd -g 1000 ${USER} && \
	useradd -m -u 1000 -g ${USER} ${USER}

########################################
##      BUILD ROS 2 DEPENDENCIES      ##
########################################
FROM base AS dep-ros2

ARG MICRO_XRCE_DDS_AGENT_VERSION=v2.4.2
ARG PX4_MSGS_VERSION=main
ARG PX4_ROS2_INTERFACE_LIB_VERSION=main
RUN --mount=type=bind,source=./docker/scripts/build_ros_deps.sh,target=/root/scripts/build_ros_deps.sh /root/scripts/build_ros_deps.sh ${MICRO_XRCE_DDS_AGENT_VERSION} ${PX4_MSGS_VERSION} ${PX4_ROS2_INTERFACE_LIB_VERSION}

########################################
##             BUILD BASE             ##
########################################
FROM base AS dev

# install built packages
COPY --from=dep-ros2 --chown=${USER}:${USER} /root/px4_ros_ws/install /home/${USER}/px4_ros_ws/install

# Switch to user
USER ${USER}

WORKDIR /home/${USER}

# install PX4-gazebo-models and QGC
RUN git clone --depth 1 https://github.com/PX4/PX4-gazebo-models.git
ADD --link --chown=${USER}:${USER} --chmod=755 https://github.com/mavlink/qgroundcontrol/releases/download/v5.0.4/QGroundControl-x86_64.AppImage QGroundControl-x86_64.AppImage

# build workshop workspace
WORKDIR /home/${USER}/roscon-25-workshop_ws/
COPY --chown=${USER}:${USER} ./px4_roscon_25 src/
RUN source /home/${USER}/px4_ros_ws/install/setup.bash && \
    colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING=ON