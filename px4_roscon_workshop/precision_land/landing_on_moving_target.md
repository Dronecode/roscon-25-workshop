# Landing on Moving Target

The launchfile [precision_land_on_moving_target.launch.py](./launch/precision_land_on_moving_target.launch.py)

- Spawns a x500 quadrotor (vehicle 1, `/px4_0`) and a differential rover (vehicle 2, `/px4_2`)
- Attaches the precision land external mode node to the x500 and the teleoperation external node to the differential rover

## Exercise

Try to automatically land the x500 above the differential rover while it moves around.

### Steps

1. Use _Teleoperation_ to command the differential rover, start with just going forward (or in cicles) at low speed.
2. Have the x500 in hover above the expected path of the rover
3. Switch the x500 into _PrecisionLandCustom_ when the rover is close
4. Does the x500 succeed in landing?

### Hints

- A QGC limitation prevents it to correctly show the external modes for both vehicles at the same time.
You will have to change mode from ROS 2.
- Instead of using the teleop_twist_rpyt_keyboad node to control the rover, consider publishing directly to the Teleop control topic.

### Even more hints (AKA solutions)

- To automatically teleop the rover you can use:

   ```sh
   ros2 topic pub -r 10 /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.1, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}"
   ```

- To change PX4 mode to _ExternalMode1_ from ROS 2 you can use:

   ```sh
   ros2 topic pub -1 /fmu/in/vehicle_command px4_msgs/msg/VehicleCommand "{
    timestamp: 0,
    param1: 23.0,
    command: 100001,
    target_system: 0,
    target_component: 0,
    source_system: 0,
    source_component: 0,
    from_external: false
    }"
   ```

   Make sure to publish to the right topic though!
