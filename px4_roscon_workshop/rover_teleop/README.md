# Rover Teleoperation

This package uses a PX4 custom mode to teleoperate a differential rover, via
[`RoverThrottleSteeringSetpointType`](https://auterion.github.io/px4-ros2-interface-lib/classpx4__ros2_1_1RoverThrottleSteeringSetpointType.html).
`cmd_vel` linear.x maps to throttle, and angular.z maps to the normalized
left/right wheel speed difference.

## Build

```sh
colcon build --symlink-install --packages-select rover_teleop
source install/setup.bash
```

## Run

Spawn the stock PX4 differential rover from your local `PX4-Autopilot`:

```sh
make px4_sitl gz_rover_differential
```

Run the teleop node:

```sh
ros2 run rover_teleop rover_teleop
```

Run the keyboard teleop:

```sh
ros2 run teleop_twist_keyboard teleop_twist_keyboard
```

Just like in the Custom Mode demo, the teleop mode requires you to manually activate it and arm the rover!

A custom, scaled-up rover model (`rover_differential_xl`) is included in
[`models/`](./models) for future use but isn't used by this workflow yet.

## Exercise

Drive the rover to the target using teleoperation.
