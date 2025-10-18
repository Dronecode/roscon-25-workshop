# PX4 ROSCON 25

This package contains a launchfile to simplify the simulations startup.

To use it, run

```sh
ros2 launch px4_roscon_25 common.launch.py
```

The launchfile will

- Start a ros_gz_bridge to get GZ clock topic in ROS 2 so that `use_sim_time` can be used.
- Start the `robot_state_publisher` to publish a simplified robot description of the x500 frame.
- Start the [`px4_tf_publisher`](../px4_tf/README.md) node.
- Start Foxglove bridge
- Start a static_ft_publisher to link `map` frame to `odom` frame. This is to account for the X500 drone initial altitude.
- Start the MicroXRCEAgent agent

You can use [Foxglove client](https://foxglove.dev/) to visualize the drone position and get other insights.
The [roscon-25-workshop](../../foxglove/roscon-25-workshop.json) layout provides 3D visualization, xy-map and altitude plot.

Gazebo, PX4 and QGC are not automatically started by this launchifle.
Please refer to [the setup](../../docs/setup.md) to know more on how to start them.
