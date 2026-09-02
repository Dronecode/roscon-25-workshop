# VTOL Executor

This package demonstrates a full VTOL mission using the PX4-ROS2 Interface Library: takeoff and land using PX4's **inbuilt** modes, and fly the fixed-wing cruise leg (including both VTOL transitions) using a **custom** mode, all orchestrated by a `VtolCustomModeExecutor`.

## Overview

- **VtolCustomMode** (`VtolCustomMode.cpp/.hpp`): a custom flight mode that owns the *entire* fixed-wing excursion, including both VTOL transitions:
  1. Commands the transition to fixed-wing via [`px4_ros2::VTOL`](https://auterion.github.io/px4-ros2-interface-lib/) and, while it's in progress, streams the required combination of setpoints every cycle (`TrajectorySetpointType` with the acceleration from `computeAccelerationSetpointDuringTransition()`, plus a `FwLateralLongitudinalSetpointType` course to realign heading).
  2. Once `VTOL::getCurrentState()` reports `FixedWing`, flies a course/altitude/airspeed setpoint via `FwLateralLongitudinalSetpointType` for a configurable duration.
  3. Commands the transition back to multicopter and streams the same setpoint combination until it completes, then calls `completed()`.

  Commanding transitions from an external mode makes *you* responsible for streaming those setpoints correctly during the transition - PX4 only does this automatically for onboard-triggered transitions (e.g. an RC switch).

- **VtolCustomModeExecutor** (`VtolCustomModeExecutor.cpp/.hpp`): a mode executor that only has to bracket `VtolCustomMode` with the inbuilt Takeoff/Land modes, since the mode itself handles both transitions.

### Flight sequence

1. **Takeoff** — inbuilt PX4 Takeoff mode (multicopter form)
2. **Cruise** — runs `VtolCustomMode`: transition to fixed-wing, cruise, transition back to multicopter
3. **ReturnToLaunch** — inbuilt PX4 RTL mode (multicopter form): flies back to the home position and lands there, rather than landing wherever the back-transition happened to finish
4. **WaitUntilDisarmed** — safe completion state

This requires a VTOL airframe in simulation, e.g. PX4's standard VTOL models (`make px4_sitl gz_standard_vtol` or similar).

## Parameters

See [`cfg/params.yaml`](./cfg/params.yaml):

- `takeoff_altitude_amsl` — altitude AMSL [m] to reach before transitioning
- `cruise_altitude_amsl` — altitude AMSL [m] to hold during cruise
- `cruise_course_deg` — compass course [deg] to fly during cruise
- `cruise_airspeed` — equivalent airspeed setpoint [m/s] during cruise
- `cruise_duration_s` — how long to cruise before transitioning back and landing

## Usage

1. Start the simulation, PX4 (VTOL airframe) and QGC as described in the [setup guide](../../docs/setup.md).
2. Start the additional ROS 2 node through the [common launchfile](../px4_roscon_workshop/README.md)

   ```sh
   ros2 launch px4_roscon_workshop common.launch.py
   ```

3. Run `vtol_executor.launch.py`

   ```sh
   ros2 launch vtol_executor vtol_executor.launch.py
   ```

4. Just like in the Custom Mode demo, this does not automatically arm the vehicle or switch modes for you:

   1. On the QGC window, select the **VtolCruise** mode.
   2. Arm the vehicle. The executor then takes over: takeoff, transition, cruise, transition back, land, disarm.

The `vtol_executor.launch.py` can also start the _MicroXrceAgent_: set the launch argument `run_uxrcedds_agent` to `true` if you don't use `common.launch.py`.

## Exercises

1. Add a parameter for cruise distance (instead of duration) using the local position estimate to decide when to transition back.
2. Make the executor abort (transition back early and land) if the cruise leg takes too long or the vehicle strays outside a geofence.
3. Replace the fixed course/altitude cruise with a short list of waypoints.
