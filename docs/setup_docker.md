# Dockerized setup

The workshop exercises can be run in a dockerized environment with minimal components having to run on the Host.

## Host setup

First of all, [install docker](https://docs.docker.com/get-started/get-docker/), then pull the `px4io/px4-dev-ros2-gazebo` image.

```bash
docker pull px4io/px4-dev-ros2-gazebo:main-jazzy
```

Then clone the PX4-Autopilot repo and checkout the `release/1.18` branch.

```bash
git clone --recurse-submodules -b release/1.18 https://github.com/PX4/PX4-Autopilot.git
```

Then create your ROS 2 workspace:

1. Create a ROS 2 workspace

    ```bash
    mkdir -p ~/workspaces/px4_roscon26_ws/src
    cd ~/workspaces/px4_roscon26_ws/src
    git clone -b develop git@github.com:Dronecode/roscon-25-workshop.git px4-roscon-workshop
    ```

1. Clone the dependencies

    ```bash
    cd ~/workspaces/px4_roscon26_ws
    vcs import src < ./src/px4-roscon-workshop/jazzy.repos
    ```

### QGroundControl

Download [QGC](https://docs.qgroundcontrol.com/master/en/qgc-user-guide/getting_started/download_and_install.html).

### Foxglove

Download [Foxglove](https://foxglove.dev/download)

## Running in docker

The `./docker/docker_run.sh` script can be used to start the container with the right mounts and envs.

```bash
./docker/docker.run ~/PX4-Autopilot
```

This will:

- Mount the `~/PX4-Autopilot` directory into `/PX4-Autopilot`.
- Mount the workspace src directory `~/workspaces/px4_roscon26_ws/src` into `/workspace/src`
- Forward GUI (software OpenGL)

Now install the workspace dependencies and build the workspace, these commands will need to be run each time the container is created and started.

```bash
apt update
rosdep install -y -i --from-paths src --skip-keys OpenCV
source /opt/ros/jazzy/setup.bash
colcon build --symlink-install
```

With the workspace build, you can now build PX4, these commands will also need to be run each time the container is created and started.

```bash
cd /PX4-Autopilot
make px4_sitl_default
```

You can now refer to [How to start the simulation](./setup.md#how-to-start-the-simulation) to validate the setup.