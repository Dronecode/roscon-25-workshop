# Development in Docker

A dockerfile is provided to allow running PX4-SITL, GZ HARMONIC and ROS 2 Humble nodes.

The ROS 2 workspace is based on the `ros:humble-perception` image. On top of it the workspace `px4_ros_ws` (`/home/$USER/px4_ros_ws`) contains PX4 dependencies such as:

- Micro-XRCE-DDS-Agent
- px4_msg
- px4-ros2-interface-lib

This should allow to build PX4 ROS 2 applications.

Gazebo Harmonic is installed and the ros-gz packages are also installed.

## Building the container

The container can be build by running

```sh
cd roscon-25-workshop
./docker/docker_build_amd64.sh
```

## Running the container

The container can either run with GUI access or in headless mode.
THe following instructions have been tested on WSL2

### Running with GUI

You can run the container with GUI enabled with

```sh
./docker/docker_run.sh
```

The script will:

- Start and run a container from `px4/roscon-25-workshop` image.
- Name it `px4-roscon-25`.
- Ensure that you can run Appimages (QGC) from inside the container.
- Ensure that GUI can be used from inside the container.
- Mount the `roscon-25-workshop/px4_roscon_25` folder in `~/roscon-25-workshop_ws/src/`.

### Running headless

You can run the container in headless mode with

```sh
./docker/docker_run_nogui.sh
```

The script will:

- Start and run a container from `px4/roscon-25-workshop` image.
- Name it `px4-roscon-25`.
- Mount the `roscon-25-workshop/px4_roscon_25` folder in `~/roscon-25-workshop_ws/src/`.

## Starting PX4 simulation

To start the PX4 simulation with Gazebo Harmonic you have to separately spin up a gazebo world. First start the container with `docker_run.sh` or `docker_run_nogui.sh` or open an new terminal in the container if it is already running.

If you want Gazebo to also open the gz client (GUI) then run

```sh
python3 /home/ubuntu/PX4-gazebo-models/simulation-gazebo --model_store /home/ubuntu/PX4-gazebo-models/
```

If instead you only want to run the gz server, then run

```sh
python3 /home/ubuntu/PX4-gazebo-models/simulation-gazebo --model_store /home/ubuntu/PX4-gazebo-models/ --headless
```

To start instead PX4 sitl, open another terminal and run

```sh
PX4_GZ_STANDALONE=1 PX4_SYS_AUTOSTART=4001 PX4_SIM_MODEL=gz_x500 /home/ubuntu/px4_sitl/bin/px4 -w /home/ubuntu/px4_sitl/romfs
```

Finally, if your're not running headless, you can open QGC and have it connect to PX4 by running

```sh
/home/ubuntu/QGroundControl/qgroundcontrol
```

from inside the container.

If instead you're running the container without GUI, then QGC must run on the host and in order for it to connect PX4 a custom UPD link needs to be crated. Add as server with IP matching the container IP and port set to `18570`.

## Developing

When the image is build, the content of `roscon-25-workshop/px4_roscon_25` is automatically copied into `/home/${USER}/roscon-25-workshop_ws/src`. The `roscon-25-workshop/px4_roscon_25` is the source folder for any ROS 2 package you which to build and right now it only contains a dummy package.

If you change your packages and you want to re-build them connect to the running container and run

```sh
cd ~/roscon-25-workshop_ws
colcon build --symlink-install
```
