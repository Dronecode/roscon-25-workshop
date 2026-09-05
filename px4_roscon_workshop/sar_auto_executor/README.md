# SAR Auto Executor

This package builds on [`sar_modes`](../sar_modes/README.md): the same **SAR (V-Sweep)** and **SAR (Orbital)** formation modes, plus a **SAR (Auto)** mode that automatically switches between them based on the rover's speed.

## Overview

`SARModeExecutor` (`px4_ros2::ModeExecutorBase`) owns a placeholder mode, **SAR (Auto)**, the only one you select in QGC. It tracks the rover's speed from consecutive `/rover/pose` messages and schedules the active mode:

- Below 0.3 m/s -> **SAR (Orbital)**
- At or above 0.6 m/s -> **SAR (V-Sweep)**
- In between -> stays in whichever mode is already active (hysteresis, to avoid flapping)

`SAR (V-Sweep)` and `SAR (Orbital)` are still independently selectable too, for manual override or debugging.

`fake_rover_mover.py` (reused from `sar_modes`) drives at two different speeds per phase, straddling both thresholds, so a single run exercises the switch both ways.

## Prerequisites

Same as [`sar_modes`](../sar_modes/README.md#prerequisites), build PX4 and the ROS 2 workspace as described in the [setup guide](../../docs/setup.md) ([dockerized setup](../../docs/setup_docker.md) if you'd rather run in a container), then build this package:

```sh
colcon build --symlink-install --packages-select sar_modes sar_auto_executor
source install/setup.bash
```

## Usage

1. Launch the exercise, brings up Gazebo, the bridge, `MicroXRCEAgent`, all 3 `x500` instances, one `sar_auto_executor` node per drone, and the fake rover:

   ```sh
   ros2 launch sar_auto_executor sar_auto_executor.launch.py
   ```

   Pass `px4_autopilot_path` if `PX4_PATH` isn't already set.

2. Arm each vehicle and take off manually, then select **SAR (Auto)**. The executor schedules V-Sweep or Orbital based on the rover's current speed, and keeps switching as it crosses the thresholds.

## Exercises

1. Make `kSpeedLowThreshold`/`kSpeedHighThreshold` ROS parameters instead of compile-time constants.
2. Add a third state to `evaluateAndSwitch()` for a rover that's been stationary for a while.
