# Offboard Demo - Example 1

This package demonstrates how to use PX4 Offboard mode from ROS 2. It showcases a complete autonomous flight mission using a state machine that controls the drone through various flight phases.

## Overview

The Offboard Demo implements a state-based flight controller that executes the following Offboard mode:

1. **Arm** - Arms the vehicle and prepares for flight
2. **Takeoff** - Takes off to a specified altitude
3. **Hover** - Hovers at takeoff position for a configurable duration
4. **Waypoint Navigation** - Flies through a predefined set of waypoints in a square pattern
5. **Yaw Rotation** - Performs a 360-degree rotation while hovering
6. **Land** - Lands the vehicle safely
7. **Disarm** - Disarms the vehicle when landed

## Usage

1. Start the simulation, PX4 and QGC as described in the [docker guide](../../docker/README.md).
2. Run `offboard_demo.launch.py` from inside the docker container

   ```sh
   docker exec -it px4-roscon-25 bash -ic "ros2 launch offboard_demo offboard_demo.launch.py run_uxrcedds_agent:=true"
   ```

The `offboard_demo.launch.py` can also start the _MicroXrceAgent_. Set the launch argument `run_uxrcedds_agent` to `true` to run it.

## Exercises

1. Change the takeoff altitude to 3 m via ROS2 parameter
2. Slow down the drone while going through the waypoints and speed up the yaw rotation

   Hint: https://docs.px4.io/main/en/flight_stack/controller_diagrams#multicopter-velocity-controller & QGC

3. Add another state, where you change the altitude to 1.5 m before going to the waypoints
