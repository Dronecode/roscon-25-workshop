########################################
##         CREATE BASE IMAGE          ##
########################################
FROM ros:jazzy-ros-base AS base

SHELL ["/bin/bash", "-o", "pipefail", "-o", "errexit", "-c"]
ENV DEBIAN_FRONTEND=noninteractive
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

# install dependencies
RUN --mount=type=bind,source=./docker/scripts/install_deps.sh,target=/root/scripts/install_deps.sh /root/scripts/install_deps.sh

ENV USER=ubuntu

ARG PX4_VERSION=v1.17.0
RUN --mount=type=bind,source=./docker/scripts/build_px4.sh,target=/home/${USER}/scripts/build_px4.sh \
    /home/${USER}/scripts/build_px4.sh ${PX4_VERSION}

########################################
##      BUILD ROS 2 DEPENDENCIES      ##
########################################
FROM base AS dep-ros2

ARG PX4_MSGS_VERSION=release/1.17
ARG PX4_ROS2_INTERFACE_LIB_VERSION=release/1.17
RUN --mount=type=bind,source=./docker/scripts/build_ros_deps.sh,target=/root/scripts/build_ros_deps.sh /root/scripts/build_ros_deps.sh ${PX4_MSGS_VERSION} ${PX4_ROS2_INTERFACE_LIB_VERSION}

# ########################################
# ##            INSTALL QGC             ##
# ########################################

# FROM base AS qgc

# ARG TARGETARCH
# ARG QGC_VERSION=v5.0.8
# RUN --mount=type=bind,source=./docker/scripts/install_qgc.sh,target=/home/${USER}/scripts/install_qgc.sh /home/${USER}/scripts/install_qgc.sh ${QGC_VERSION} ${TARGETARCH}

########################################
##             BUILD DEV             ##
########################################
FROM base AS dev

# install built packages
COPY --from=dep-ros2 --chown=${USER}:${USER} /root/px4_ros_ws/install /home/${USER}/px4_ros_ws/install

# # retrieve QGC
# COPY --from=qgc --chown=${USER}:${USER} /home/${USER}/QGroundControl /home/${USER}/QGroundControl

# Switch to user
USER ${USER}

WORKDIR /home/${USER}

RUN cd PX4-Autopilot && \
    make px4_sitl_zenoh

ENV RMW_IMPLEMENTATION=rmw_zenoh_cpp

# # retrieve PX4-gazebo-models
# COPY --from=build-px4 --chown=${USER}:${USER} /home/${USER}/PX4-Autopilot/Tools/simulation/gz /home/${USER}/PX4-gazebo-models

# # build workshop workspace
# WORKDIR /home/${USER}/roscon-25-workshop_ws/
# COPY --chown=${USER}:${USER} . src/roscon-25-workshop
# RUN source /home/${USER}/px4_ros_ws/install/setup.bash && \
#     colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING=ON

# WORKDIR /home/${USER}/
# COPY --from=build-px4 --chown=${USER}:${USER} /home/${USER}/px4_sitl /home/${USER}/px4_sitl
# RUN echo "source /home/$USER/roscon-25-workshop_ws/install/setup.bash" >> /home/$USER/.bashrc && \
#     echo "source /usr/share/colcon_argcomplete/hook/colcon-argcomplete.bash" >> /home/$USER/.bashrc
