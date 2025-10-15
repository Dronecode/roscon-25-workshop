# Precision Landing with Executor Demo

This package demonstrates how to create and use PX4 custom flight modes from ROS 2 using the PX4-ROS2 Interface Library.
It showcases the implementation of custom waypoints and precision landing behaviors that integrate seamlessly with PX4's mode management system.

## Overview

The Precision Landing Executor Demo implements two custom flight modes using the PX4-ROS2 Interface Library:

1. **CustomWaypoints Mode**: Autonomous waypoint navigation with predefined trajectory
2. **PrecisionLand Mode**: Precision landing using visual target tracking

These modes are orchestrated by an **Executor** that manages the complete flight sequence through a state machine architecture.
You have to select CustomWaypoints for the sequence to start.

### Flight Sequence

The demo executes the following autonomous sequence:

1. **Takeoff** - Automatic takeoff to predefined altitude
2. **CustomWaypoints** - Navigate through rectangular waypoint pattern
3. **PrecisionLand** - Perform precision landing using visual guidance
4. **WaitUntilDisarmed** - Safe completion state

## Precision Landing

The precision landing mode demonstrates autonomous landing using visual target detection and tracking.
The system:

- Detects visual landing targets (ArUco markers or other fiducials)
- Calculates precise position corrections relative to the target
- Executes controlled descent while maintaining position over the target
- Provides smooth and accurate landing on the designated target

## Prerequisites

1. Start the simulation, PX4 and QGC as described in the [docker guide](../../docker/README.md) with the ArUco marker and the drone with the camera:

   ```sh
   docker exec -it px4-roscon-25 /bin/bash -c "PX4_GZ_STANDALONE=1 PX4_SYS_AUTOSTART=4014 /home/ubuntu/px4_sitl/bin/px4 -w /home/ubuntu/px4_sitl/romfs"
   ```

2. Ensure the vehicle is armed (GPS lock, all sensors healthy)
3. Verify QGroundControl connection for mode monitoring
4. Set up visual landing target in the simulation environment

## Usage

1. Start the simulation, PX4 and QGC as described in the [docker guide](../../docker/README.md).
2. Start the ArUco tracker node:

   ```sh
   ros2 launch aruco_tracker aruco_tracker.launch.py run_uxrcedds_agent:=true
   ```

3. Run `precision_land_executor.launch.py` from inside the docker container

   ```sh
   ros2 launch precision_land_executor precision_land_executor.launch.py run_uxrcedds_agent:=true
   ```

The `precision_land_executor.launch.py` can also start the _MicroXrceAgent_. Set the launch argument `run_uxrcedds_agent` to `true` to run it.
4. Select **CustomWaypoints** and arm the drone
