# Gimbal control

This demo shows how one can directly interact with PX4 from ROS 2 bypassing the interface library.
As a case study, the control of pan and tilt of a camera attached under a standard VTOL is considered.

The camera is mounted on a three-axis gimbal and simulated in Gazebo with the augmented standard-vtol model [vtol_gimbal](../px4_roscon_workshop/models/vtol_gimbal/model.config).
PX4 low-level drives the gimbal through the PX4 `gz-bridge` module and let other modules control it by:

1. Implementing the [Gimbal Protocol (v2)](https://mavlink.io/en/services/gimbal_v2.html).
The PX4 `gz-bridge` acts as Gimbal Device.
1. Subscribing to the PX4 `gimbal_controls` topic.
The `GimbalControl.mgs` captures the desired gimbal roll,yaw,pitch angles:

    ```msg
    uint64 timestamp			# time since system start (microseconds)
    uint8 INDEX_ROLL = 0
    uint8 INDEX_PITCH = 1
    uint8 INDEX_YAW = 2

    uint64 timestamp_sample	    # the timestamp the data this control response is based on was sampled
    float32[3] control	# Normalized output. 1 means maximum positive position. -1 maximum negative position. 0 means no deflection. NaN maps to disarmed.
    ```

We will use the second approach.

## Configuration

PX4 `gimbal_controls` topic is not bridged by default over ROS 2.
Therefore the bridge configuration file needs to be edited, see [PX4 dds topic](https://docs.px4.io/main/en/middleware/uxrce_dds#dds-topics-yaml) documentation to learn how to add `gimbal_controls` to the list of PX4 outbound topics, then rebuild PX4 SITL.

## gimbal_teleop

The gimbal teleop node converts standard ROS 2 `geometry_msgs::msg::Twist` messages into the PX4 specific `px4_msgs::msg::GimbalControls` messages and directly publishes on `"fmu/in/gimbal_controls"`.

- The `linear.x` component of the input topic (`"gimbal/cmd_vel"`) is interpreted as desired **pan/yaw** rate.
- The `linear.y` component is instead interpreted as desired **tilt/pitch** rate.
