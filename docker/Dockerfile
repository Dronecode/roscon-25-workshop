########################################
##         CREATE BASE IMAGE          ##
########################################
FROM ros:humble-ros-base AS base

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
##           BUILD OPENCV             ##
########################################

FROM base AS build-opencv

ARG OPENCV_VERSION=4.10.0
RUN --mount=type=bind,source=./docker/scripts/build_opencv.sh,target=/home/${USER}/scripts/build_opencv.sh /home/${USER}/scripts/build_opencv.sh ${OPENCV_VERSION}

########################################
##           BUILD PRE-DEV            ##
########################################

FROM base AS pre-dev

COPY --from=build-opencv /OpenCV/install /usr/local

########################################
##      BUILD ROS 2 DEPENDENCIES      ##
########################################
FROM pre-dev AS dep-ros2

ARG MICRO_XRCE_DDS_AGENT_VERSION=v2.4.2
ARG PX4_MSGS_VERSION=release/1.16
ARG PX4_ROS2_INTERFACE_LIB_VERSION=release/1.16
RUN --mount=type=bind,source=./docker/scripts/build_ros_deps.sh,target=/root/scripts/build_ros_deps.sh /root/scripts/build_ros_deps.sh ${MICRO_XRCE_DDS_AGENT_VERSION} ${PX4_MSGS_VERSION} ${PX4_ROS2_INTERFACE_LIB_VERSION}

########################################
##              BUILD PX4             ##
########################################

FROM pre-dev AS build-px4

ARG PX4_VERSION=v1.16.0
RUN --mount=type=bind,source=./docker/scripts/build_px4.sh,target=/home/${USER}/scripts/build_px4.sh /home/${USER}/scripts/build_px4.sh ${PX4_VERSION}

########################################
##            INSTALL QGC             ##
########################################

FROM base AS qgc

ARG QGC_VERSION=v5.0.6
RUN --mount=type=bind,source=./docker/scripts/install_qgc.sh,target=/home/${USER}/scripts/install_qgc.sh /home/${USER}/scripts/install_qgc.sh ${QGC_VERSION}

########################################
##             BUILD DEV             ##
########################################
FROM pre-dev AS dev

# install built packages
COPY --from=dep-ros2 --chown=${USER}:${USER} /root/px4_ros_ws/install /home/${USER}/px4_ros_ws/install

# retrieve QGC
COPY --from=qgc --chown=${USER}:${USER} /home/${USER}/QGroundControl /home/${USER}/QGroundControl

# Switch to user
USER ${USER}

WORKDIR /home/${USER}

# retrieve PX4-gazebo-models
COPY --from=build-px4 --chown=${USER}:${USER} /home/${USER}/PX4-Autopilot/Tools/simulation/gz /home/${USER}/PX4-gazebo-models

# build workshop workspace
WORKDIR /home/${USER}/roscon-25-workshop_ws/
COPY --chown=${USER}:${USER} ./px4_roscon_25 src/
RUN source /home/${USER}/px4_ros_ws/install/setup.bash && \
    colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING=ON

WORKDIR /home/${USER}/
COPY --from=build-px4 --chown=${USER}:${USER} /home/${USER}/px4_sitl /home/${USER}/px4_sitl
COPY ./docker/entrypoint.sh /ros_entrypoint.sh

ENTRYPOINT [ "/ros_entrypoint.sh" ]