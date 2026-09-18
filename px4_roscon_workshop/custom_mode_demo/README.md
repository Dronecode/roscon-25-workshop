# Custom Mode Demo

This package demonstrates how to create and register a PX4 custom flight mode from ROS 2 using the PX4-ROS2 Interface Library.

## Overview

The Custom Mode Demo registers one mode, **CustomWaypoints**, which flies a predefined rectangular waypoint trajectory when selected in QGroundControl.

## Prerequisites

1. Start the simulation, PX4 and QGC as described in the [setup guide](../../docs/setup.md).
2. Ensure the vehicle is armed (GPS lock, all sensors healthy)
3. Verify QGroundControl connection for mode monitoring

## Usage

1. Start the simulation, PX4 and QGC as described in the [setup guide](../../docs/setup.md).
2. Start the additional ROS 2 node through the [common launchfile](../px4_roscon_workshop/README.md)

   ```sh
   ros2 launch px4_roscon_workshop common.launch.py
   ```

3. Run `custom_mode_demo.launch.py`

   ```sh
   ros2 launch custom_mode_demo custom_mode_demo.launch.py
   ```

4. Use QGC to takeoff, then activate the custom **CustomWaypoints** mode.
