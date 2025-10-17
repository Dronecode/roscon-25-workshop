<h1 align="center">Hands-On Aerial Robotics Using PX4 and ROS 2</h1>

<p align="center">
    <strong>Fly with ROS 2. Powered by PX4</strong>
</p>
<p align="center">
    <a href="https://creativecommons.org/licenses/by-sa/4.0/">
        <img src="https://img.shields.io/badge/License-CC_BY--SA_4.0-lightgrey.svg" alt="License: CC BY-SA 4.0">
    </a>
</p>

## About
Picture yourself in Gazebo, sending velocity, position, and acceleration commands straight from your ROS 2 nodes while PX4 quietly manages real-time flight control and built-in safety. Over the course of a full-day, hands-on workshop, you'll see why PX4 powers so many high-reliability robotics projects, learn to hook your existing ROS 2 setup into our plug-and-play bridge, and stream live telemetry back to your code. You'll explore perception integrations, think ArUco markers and LiDAR, build high-level control routines in ROS2, and leave with working simulation environments, example projects, and a clear path to swap out virtual drones for real hardware. No PX4 background is needed, just your ROS expertise, a laptop, and a spirit of discovery. Everything in this workshop is fully open source and open for you to inspect, modify, and share. 

This repository contains all the materials for the ROSCon 2025, Hands-On Aerial Robotics Using PX4 and ROS 2 Workshop.
For questions regarding the workshop, please join the Dronecode Foundation Discord and post your question in the workshop-roscon-2025 channel:

[Join the Dronecode Foundation Discord](https://discord.gg/dhPwqRVD)


## Outline
This workshop introduces you to PX4’s ROS 2 integration layer and shows how to create your own flight modes, perception pipelines, and control executors.
You’ll also get a brief introduction to PX4 and a ready-to-use developer environment designed for seamless integration between PX4 and ROS 2.
By the end of the workshop, you’ll have a complete ROS 2 package capable of controlling a simulated drone, performing navigation tasks, and executing precision landings.
**Note:** Each example includes several exercises designed to reinforce the concepts, deepen understanding, and encourage exploration of the environment. Solutions are provided either **commented out in the code** (you can uncomment and rebuild the package to test) or as **separate, individual packages.**

For more detailed instructions and guidance, please refer to the dedicated **README** for each example.

### Introduction & Drone Architecture
[PX4 Documentation](https://docs.px4.io/main/en/)

### Environment Setup
For detailed environment and Docker setup instructions, see the [docs/README.md](docs/README.md) guide.

### Control Pipelines
There are two main ways to interact with PX4 and ROS 2:

**Offboard Mode** – the classic method for sending velocity or position setpoints directly to PX4.

**PX4 ROS 2 Interface / Custom Modes** – the newer method using the px4_ros2 library, allowing you to create custom flight modes and executors.

In this section, we demonstrate a simple flight sequence: Takeoff → Waypoints → Yaw → Landing.
The goal is to compare these two approaches, highlighting their differences and advantages.

For detailed instructions and exercises, refer to the following guides in this repository:
- **Offboard Demo:** [Offboard Demo](offboard_demo/README.md)  
- **Custom Mode Demo:** [Custom Mode Demo](custom_mode_demo/README.md)

### Perception & Applications

In this section, we explore **three practical examples** of perception and control in ROS 2 with PX4:

1. **ArUco Marker Detection** – Detect markers using ROS 2 and PX4. No custom flight mode is required.  
2. **Teleoperation** – Ever seen a TurtleBot flying? This demo shows how to manually control a drone using a keyboard and to use a LiDAR scan for environmental awareness.  
3. **Precision Landing** – Combine ArUco detection with a **Custom Mode** to perform precision landing.

For more detailed instructions and exercises, refer to the following demos:

- **ArUco Marker Detection:** [ArUco Marker Detection](aruco_tracker/README)  
- **Teleoperation:** [Teleoperation](teleop/README.md)  
- **Precision Landing:** [Precision Landing](precision_land/README.md)

### Q&A, Resources & Hardware Show-and-Tell

At the end of the workshop, we provide dedicated time for questions, troubleshooting, and hands-on guidance with hardware.

### Open Lab & Individual Consultations 

Dedicated time for leftover exercises and indivisual consultation.

