# teleop

This packages offers uses PX4 custom modes and executor to teleop a drone.

## Usage

Launch gz [walls](https://github.com/PX4/PX4-gazebo-models/blob/e05f4312d3f28aa621157610584a4870406cb6d3/worlds/walls.sdf) world, you'll have to navigate inside narrow passages now.

First start the container, then run

```sh
python3 /home/ubuntu/PX4-gazebo-models/simulation-gazebo --model_store /home/ubuntu/PX4-gazebo-models/ --world walls
```

Spawn model and attach PX4 SITL

```sh
PX4_GZ_STANDALONE=1 PX4_SYS_AUTOSTART=4013 PX4_PARAM_UXRCE_DDS_SYNCT=0 /home/ubuntu/px4_sitl/bin/px4 -w /home/ubuntu/px4_sitl/romfs
```

Here we used `PX4_SYS_AUTOSTART=4013` which set the Gazebo model to _x500_lidar_2d_.
Don't forget to start QGC too.

When the simulation is running you can see the GZ topics:

```sh
gz topic -l
```

You can now launch the [common](../px4_roscon_25/README.md) launchfile

```sh
ros2 launch px4_roscon_25 common.launch.py
```

and finally the `teleop` launchfile and the _keyboard_monitor_ node.

```sh
ros2 launch teleop teleop.launch.py
```

```sh
ros2 run teleop_twist_rpyt_keyboard teleop_twist_rpyt_keyboard
```
