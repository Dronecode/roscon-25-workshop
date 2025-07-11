<h1 align="center">🚀 PX4 + ROS 2 ROSCon 2025 Workshop</h1>

<p align="center">
    <strong>Fly with ROS 2. Powered by PX4</strong>
</p>
<p align="center">
    <a href="https://creativecommons.org/licenses/by-sa/4.0/">
        <img src="https://img.shields.io/badge/License-CC_BY--SA_4.0-lightgrey.svg" alt="License: CC BY-SA 4.0">
    </a>
</p>

## About
This full-day workshop is designed for ROS 2 developers ready to dive into aerial robotics with PX4. You’ll connect ROS 2 nodes to PX4’s real-time flight stack, stream telemetry into RViz, build offboard control logic, and integrate perception using Gazebo, ArUco markers, and LiDAR. By the end, you’ll have a complete simulation environment, reusable code, and a path to real hardware—no prior PX4 knowledge required.

This repository contains all the materials for the ROSCon 2025 PX4 + ROS 2 Workshop.

## Outline

### Introduction & Drone Architecture
### Environment Setup
### Control Pipelines
#### Offboard Demo
Build the workspace
```
colcon build
source install/setup.bash
```

Launch the PX4 SITL environment with Gazebo and DDS Agent and QGC
```
make px4_sitl gz_x500
MicroXRCEAgent udp4 -p 8888
```

Run the Offboard demo node
```
ros2 run offboard_demo offboard_demo
```
#### Custom Mode Demo
```
git submodule update --init --recursive
```
TODO: For some reason px4_msgs and px4_ros2_cpp has to be built before the other packages so first:
```
colcon build --packages-select px4_msgs px4_ros2_cpp
```
then 
```
colcon build
```
```
source install/setup.bash
```
Launch the PX4 SITL environment with Gazebo and DDS Agent and QGC
```
make px4_sitl gz_x500
MicroXRCEAgent udp4 -p 8888
```
Run the Custom demo node
```
ros2 run custom_mode_demo custom_mode_demo 
```
Arm the drone in QGC and select the CustomWaypoint mode



### Perception & Application
### External Resources