# Formation Control

The `px4_formation_control` package demonstrates multi-vehicle formation control with PX4 and ROS 2. It launches three simulated X500 vehicles and runs one `Formation` flight mode per vehicle.

Each controller:

- publishes the vehicle's local TF frames (`<prefix>map` and `<prefix>base_link`),
- reads the relative TF transform to its configured neighbors, and
- commands a horizontal velocity proportional to the error from each desired neighbor distance.

The default formation is a triangle with 10 m target distances. The vehicles hold an altitude of 2 m above the local origin.

## Prerequisites

- A sourced ROS 2 installation with the workspace dependencies available.
- A built PX4-Autopilot SITL target. The launch file uses the executable at `build/px4_sitl_default/bin/px4`.
- Gazebo and the PX4 ROS 2 integration packages installed and built in this workspace.
- QGroundControl, or another way to activate the flight mode and arm the vehicles.

The PX4 repository path can be supplied explicitly, or through the `PX4_PATH` environment variable. If neither is set, the default is `~/PX4-Autopilot`.

## Build

From the workspace root:

```sh
source /opt/ros/$ROS_DISTRO/setup.bash
colcon build --packages-select px4_formation_control
source install/setup.bash
```

## Run

Launch the simulation and all three formation controllers:

```sh
ros2 launch px4_formation_control formation.launch.py \
	px4_autopilot_path:=~/PX4-Autopilot
```

The `px4_autopilot_path` argument must point to the PX4-Autopilot repository root. The launch file also accepts a Gazebo world name:

```sh
ros2 launch px4_formation_control formation.launch.py \
	px4_autopilot_path:=~/PX4-Autopilot \
	world:=default
```

After startup, use QGroundControl to activate the required mode and arm the vehicles according to the PX4 safety rules. Arming is allowed while the formation mode is active.

## Executor exercise

The package includes three executable variants:

- `px4_formation_control`: the original formation mode. Takeoff then activate `Formation` manually.
- `px4_formation_control_executor_exercise`: the workshop exercise. Complete the node by creating and registering a `px4_ros2::ModeExecutorBase` that takes off and then schedules the formation mode.
- `px4_formation_control_executor_solution`: the completed exercise. Its executor starts with a 2 m takeoff, activates the formation mode when takeoff completes, and waits for disarming when the formation mode finishes.

The launch file selects the solution by default. To try another variant, change `ACTIVE_EXECUTABLE_IDX` in `launch/formation.launch.py`:

```python
FORMATION_CONTROL_EXECUTABLE = (
	'px4_formation_control',
	'px4_formation_control_executor_exercise',
	'px4_formation_control_executor_solution',
)
ACTIVE_EXECUTABLE_IDX = 1  # exercise
```

## Default configuration

| Vehicle | Namespace | Spawn position (m) | Neighbors |
| --- | --- | --- | --- |
| 0 | `/px4_0/` | `(0, 0, 0.3)` | `px4_1`, `px4_2` |
| 1 | `/px4_1/` | `(10, 0, 0.3)` | `px4_0`, `px4_2` |
| 2 | `/px4_2/` | `(5, 8.660254, 0.3)` | `px4_0`, `px4_1` |

Each controller uses a target distance of `10.0` m and a control gain of `1.0`. These values are configured in `launch/formation.launch.py`.

## Notes

- This is an experimental workshop demonstration intended for simulation.
- The controller depends on valid local-position data and TF transforms from the other vehicles. It skips a neighbor while its transform is unavailable.
- Stop the launch process before restarting it to avoid leaving PX4 or Gazebo processes running.
