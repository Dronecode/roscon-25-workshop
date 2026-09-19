# Rover Teleoperation

This package uses a PX4 custom mode to teleoperate a differential rover, via
[`RoverThrottleSteeringSetpointType`](https://auterion.github.io/px4-ros2-interface-lib/classpx4__ros2_1_1RoverThrottleSteeringSetpointType.html).
`cmd_vel` linear.x maps to throttle, and angular.z maps to the normalized
left/right wheel speed difference.

## Build

```sh
colcon build --symlink-install --packages-up-to rover_teleop
source install/setup.bash
```

## Run

Spawn the scaled-up PX4 differential rover and launch the teleop:

```sh
ros2 launch rover_teleop rover_launch.py
```

use the launch argument `px4_autopilot_path` to provide the path to the local PX4-Autopilot repo.

When using ROS 2 Jazzy, start the gazebo GUI with `GZ_IP=127.0.0.1 gz sim -g`.
`GZ_IP=127.0.0.1` forces gz-transport discovery over loopback; without it,
on machines whose default route is Wi-Fi, the GUI can start but stay empty
because it never discovers the running server.

Run the keyboard teleop:

```sh
ros2 run teleop_twist_keyboard teleop_twist_keyboard
```

Just like in the Custom Mode demo, the teleop mode requires you to manually activate it and arm the rover!

A custom, scaled-up rover model (`rover_differential_xl`) is included in
[`models/`](./models).

## Exercise

Drive the rover to the target using teleoperation.
