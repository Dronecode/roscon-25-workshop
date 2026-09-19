# SAR Modes

This package implements Search and Rescue (SAR) swarm formation modes for PX4 using the PX4-ROS2 Interface Library. Three drones track a moving rover and hold a formation around it.

## Overview

`BaseSARMode` subscribes to `/rover/pose` (ENU) and converts it to NED for the derived modes:

- **SAR (V-Sweep)**: holds a V/wedge formation around the target, rotated to match its heading.
- **SAR (Orbital)**: circles the target at a fixed radius, drones stacked at different altitudes, each yawing to face inward.

Both modes are registered independently (no executor), so they show up as two separately selectable flight modes in QGroundControl on each vehicle.

Spawn plumbing is shared with [`formation_control`](../formation_control/README.md); however, the coordination logic is not, because SAR tracks one external target, and `formation_control` tracks its neighbors.

## Prerequisites

This exercise runs 3 vehicles at once.

1. Build PX4 and the ROS 2 workspace as described in the [setup guide](../../docs/setup.md) ([dockerized setup](../../docs/setup_docker.md) if you'd rather run in a container). `sar_modes.launch.py` starts Gazebo and all 3 PX4 instances itself, so the setup guide's own simulation startup steps aren't needed here.
2. QGroundControl (or `commander`, see below) to arm and switch modes on the vehicles.

## Usage

1. Launch the exercise. This brings up Gazebo, the ROS-GZ bridge, `MicroXRCEAgent`, all 3 `x500` instances, one `sar_modes` node per drone, and the fake rover:

   ```sh
   ros2 launch sar_modes sar_modes.launch.py
   ```

   Pass `px4_autopilot_path` if `PX4_PATH` isn't already set.

2. In QGroundControl, switch to each vehicle, arm, and take off manually, there is no scripted takeoff.
3. Select **SAR (V-Sweep)** or **SAR (Orbital)** from the flight-mode dropdown.

Check the rover is streaming with:

```sh
ros2 topic echo /rover/pose
```

## Default configuration

| PX4 instance | Namespace | `drone_id` |
| --- | --- | --- |
| 1 | `/px4_1/` | 0 |
| 2 | `/px4_2/` | 1 |
| 3 | `/px4_3/` | 2 |

PX4 instances are 1-indexed (this repo's convention, see [`px4_tf/README.md`](../px4_tf/README.md)); `drone_id` stays 0-indexed for the formation math.

## Exercises

1. Retune `SARVSweepMode`'s spacing constants or `SAROrbitalMode`'s altitude step and rebuild.
2. Wire up `add_on_set_parameters_callback` on `SAROrbitalMode` so `ros2 param set` can change its orbit radius/omega while armed, the `_param_mutex` is already there for it.
3. Turn `SAROrbitalMode`'s per-drone altitude offset into deliberate multi-resolution recon: low drones close and detailed, one drone high and wide for context.
4. Port [`formation_control`](../formation_control/README.md)'s spring based separation controller into `sar_modes` as a minimum separation correction between drones.
   Hint: `sar_modes` has no inter-drone awareness yet, you'll need to add TF broadcasting/lookup like `formation_control` does.

The completed executor exercise can be found in a separate package: [SAR Auto Executor](../sar_auto_executor/README.md).
