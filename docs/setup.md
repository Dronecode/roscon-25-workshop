# Setup

This page will guide you through the installation of the requirements for running the workshop exercises and it will explain how ROS 2, Gazebo and PX4 will interact.

The three main components are:

- [GZ HARMONIC](https://gazebosim.org/docs/harmonic/getstarted/)
- [ROS 2 Jazzy](https://docs.ros.org/en/jazzy/index.html)
- [PX4](https://github.com/PX4/PX4-Autopilot) v1.18.0 simulator

In addition to them, two extra components are required for visualization and remote control:

- **QGroundControl**. [GQC](https://qgroundcontrol.com/) provides intuitive operator control of PX4 drones, it lets you configure PX4, calibrate the drone sensors and plan mission.
QGC is already installed in the Docker images.
However, it requires GUI to enabled for the container.
If this is not possible (currently for MAC) then QGC will have to be installed on the host system.
- **Foxglove**. [Foxglove](https://foxglove.dev/download) will make visualizing the drone state and perceived environment a more user friendly way.

## Prerequisites

All the instructions and all the provided scripts have been tested on Ubuntu 24.04.

### PX4 SITL

Refer to [PX4 Gazebo SITL documentation](https://docs.px4.io/main/en/sim_gazebo_gz/) to clone PX4 repo, install the PX4 build dependencies, build the project and start a Gazebo Simulation.

**note**: PX4 Gazebo simulation with cameras you might encounter a memory leak due caused by the PX4 gstreamer plugin.
See https://github.com/PX4/PX4-Autopilot/issues/27296
Disable the plugin if you see it happening.

### QGroundControl

Download QGC

### ROS 2 workspace setup

1. Create a ROS 2 workspace
   
    ```bash
    mkdir -p ~/workspaces/px4_roscon26_ws/src
    cd ~/workspaces/px4_roscon26_ws/src
    git clone -b develop git@github.com:Dronecode/roscon-25-workshop.git
    ```

1. Install ROS 2 Jazzy: https://docs.ros.org/en/jazzy/Installation/Ubuntu-Install-Debs.html.
Pick `ros-jazzy-desktop`.
1. Install the ROS 2 developer tools

    ```bash
    sudo apt install ros-dev-tools
    ```

1. Clone the dependencies
   
    ```bash
    cd ~/workspaces/px4_roscon26_ws
    vcs import src < ./src/roscon-25-workshop/jazzy.repos
    ```

1. Install dependencies
   
    ```bash
    cd ~/workspaces/px4_roscon26_ws
    source /opt/ros/jazzy/setup.bash
    rosdep install -i --from-paths src --skip-keys OpenCV
    ```

1. Build the workspace
   
    ```bash
    cd ~/workspaces/px4_roscon26_ws
    source /opt/ros/jazzy/setup.bash
    colcon build --symlink-install
    ```

## How to start the simulation

### Starting the PX4-GZ simulation

PX4 can directly connect to GZ using the `gz-transport` libraries.
This means that PX4 can control any GZ model as long as the model uses the required sensor and actuation plugins.

For this workshop we will use the x500 quadrotor model.

Change folder to where PX4 was cloned and run

```sh
make px4_sitl gz_x500
```

The Gazebo GUI window will open

![GZ world with x500 spawned](./assets/gz_world_with_x500.png)

Before taking off you just need to connect QGC to your simulated drone.

Simply start QGC, open a terminal into the folder where you downloaded it and run

```sh
./QGroundControl-x86_64.AppImage
```

On the PX4 terminal you will see the message

```sh
INFO  [mavlink] partner IP: 172.17.0.1
INFO  [commander] Ready for takeoff!
```

This is all you need to do to start the GZ + PX4 simulation, you can now takeoff!

## TO-DO: update everything below this point

## Next step (optional) - Link the simulation to ROS 2

With the simulation up an running, it is time to bridge ROS 2 with Gazebo and PX4.

The following sections will demo the essential steps in this process.
However, when trying the exercises you can leverage the [common launchfile](../px4_roscon_workshop/px4_roscon_workshop/README.md) which automatically sets up the required bridges.

1. **Clock bridging.**  We want to leverage the GZ clock and use it to time all our ROS 2 node.
This is accomplished by first creating an unidirectional bridge between the gz `/clock` topic and the ROS 2 one and then by commanding all ROS 2 to use the newly created `/clock` ROS 2 topic as time reference.
We will use the `ros_gz_bridge` package to create the bridge:

    ```sh
    ros2 run ros_gz_bridge parameter_bridge /clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock
    ```

    While the ROS 2 behavior will be set by the parameter `use_sim_time`.
2. **ROS 2 - PX4 bridge.** PX4 leverages [eProsima Micro XRCE-DDS](https://micro-xrce-dds.docs.eprosima.com/en/v2.4.3/) which internal [PX4 messages](https://docs.px4.io/v1.16/en/middleware/uorb) to be directly exposed to the ROS 2 network.
The simulated PX4 instance automatically start the Micro XRCE-DDS client using UDP protocol on port 8888, what we need to do is to just start the agent with the same settings.

    ```sh
    MicroXRCEAgent udp4 -p 8888
    ```

    after running this command you can see PX4 establish the connection - the expected output is a sequence of messages like

    ```sh
    INFO  [uxrce_dds_client] successfully created rt/fmu/out/vehicle_status_v1 data writer, topic id: 279
    INFO  [uxrce_dds_client] successfully created rt/fmu/out/airspeed_validated data writer, topic id: 14
    INFO  [uxrce_dds_client] successfully created rt/fmu/out/vtol_vehicle_status data writer, topic id: 288
    INFO  [uxrce_dds_client] successfully created rt/fmu/out/home_position data writer, topic id: 123
    ```

### Inspecting PX4 messages

Now that the [PX4 messages](https://docs.px4.io/v1.16/en/msg_docs/) are available to ROS 2, you can list them with

```sh
ros2 topic list
```

The messages in the topics with namespace `/fmu/in` are sent from ROS 2 to PX4 while the ones with namespace `/fmu/out` go from PX4 to ROS 2.

For example, you can check the PX4 vehicle status with

```sh
ros2 topic echo /fmu/out/vehicle_status_v4
```

### Foxglove visualization

You can use the [px4_tf](../px4_roscon_workshop/px4_tf/README.md) packages, in conjunction with `foxglove_bridge` to visualize in 3D the drone `base_link`.

The `px4_tf_publisher` node subscribes to PX4 `/fmu/out/vehicle_odometry` topic and publishes a derived transform for the `odom` frame to the `base_link` frame.

```sh
ros2 run px4_tf px4_tf_publisher --ros-args -p use_sim_time:=true
```

Finally `foxglove_bridge` let's us visualize the tf in Foxglove.

```sh
ros2 run foxglove_bridge foxglove_bridge --ros-args -p use_sim_time:=true
```

Launch your Foxglove client and open a connection of type _Foxglove WebSocket_ with url `ws://localhost:8765`.

![foxglove example](./assets/foxglove.png)

**Note:** when restarting the simulations and the foxglove_bridge, you might have to restart Foxglove client too to re-establish the connection.

### Recompiling the ROS 2 workspace

To recompile the ROS 2 workspace

```sh
cd ~/roscon-25-workshop_ws/
source source ~/px4_ros_ws/install/setup.bash
colcon build --symlink-install
```

## Troubleshooting

### T1: Gazebo GUI not showing

A1: Make sure you're running the container with GPU support.

### T2: on WSL2 I'm getting `docker: Error response from daemon: error gathering device information while adding custom device "/dev/dri": no such file or directory`

A2: Only `./docker/docker_run.sh --nvidia` combined with NVIDIA Container Toolkit works out of the box on WSL2.
If you don't have nvidia drivers or NVIDIA Container Toolkit installed on WSL2 you can run it headless `./docker/docker_run.sh --no-gui` or you can try removing `DOCKER_CMD="$DOCKER_CMD --device /dev/dri:/dev/dri"` from `./docker/docker_run.sh`.
