# ROS2 & PX4 ArUco Detection using OpenCV

Explore the integration of ROS 2, PX4, and OpenCV for advanced ArUco marker detection. In this tutorial, we demonstrate how to leverage ROS 2's powerful communication framework and PX4's flight control capabilities to implement precise marker detection using OpenCV. Learn how to set up your environment, process camera feeds, and detect ArUco markers in real-time, enabling enhanced navigation and interaction for autonomous drones and robotic systems. Whether you're a beginner or an experienced developer, this tutorial will guide you through the essential steps to achieve robust marker detection and seamless integration with your ROS 2 and PX4 projects.

## ArUco Markers

ArUco markers are square fiducial markers used in computer vision for tasks like pose estimation, camera calibration, and augmented reality (AR). Each marker has a unique binary pattern inside a black border, allowing it to be easily detected and identified. They help in determining the position and orientation of cameras or objects in a scene, making them valuable in robotics, navigation, and AR applications.
https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html

## Run the simulation environment

After starting the container, we will start Gazebo loading the [`aruco`](https://github.com/PX4/PX4-gazebo-models/blob/e05f4312d3f28aa621157610584a4870406cb6d3/worlds/aruco.sdf) world:

```sh
python3 /home/ubuntu/PX4-gazebo-models/simulation-gazebo --model_store /home/ubuntu/PX4-gazebo-models/ --world aruco
```

Then we spawn the drone and connect PX4 to it.

```sh
PX4_GZ_STANDALONE=1 PX4_SYS_AUTOSTART=4014 PX4_PARAM_UXRCE_DDS_SYNCT=0 /home/ubuntu/px4_sitl/bin/px4 -w /home/ubuntu/px4_sitl/romfs
```

Here we used `PX4_SYS_AUTOSTART=4014` which set the Gazebo model to [x500_mono_came_down](https://github.com/PX4/PX4-gazebo-models/tree/e05f4312d3f28aa621157610584a4870406cb6d3/models/x500_mono_cam_down).
Don't forget to start QGC too.

When the simulation is running you can see the GZ topics:

```sh
gz topic -l
```

You can now launch the [common](../px4_roscon_25/README.md) launchfile

```sh
ros2 launch px4_roscon_25 common.launch.py
```

and then the ArUco tracker launchfile.

```sh
ros2 launch aruco_tracker aruco_tracker.launch.py world_name:=aruco model_name:=x500_mono_cam_down_0
```

You can see the images from Foxglove while the estimated ArUco pose is available on the topic `/target_pose`.

## Exercise

In this exercise, we estimate the real-world size of a detected ArUco marker using only the camera image, its intrinsic parameters, and the vehicle's measured altitude. Show this estimated marker size on the annotated image.

### Marker Size Estimation Formula

The estimated marker size can be calculated using the following formula:

```math
L_{est} = d \frac{p}{f}
```

Where:

- $L_{est}$: Estimated real-world size of the marker (in meters)
- $p$: Pixel width of the detected marker in the image (in pixels)
- $f$: Focal length of the camera (in pixels)
- $d$: Distance from camera to the marker/ground (in meters)

### Implementation Details

To access the vehicle's altitude (distance to ground), the node subscribes to PX4's `VehicleLocalPosition` message and uses the `dist_bottom` field, which provides the distance measurement from the vehicle to the ground below.

The solution to the exercise is commented out at the end of the `OffboardDemo.cpp`, `OffboardDemo.hpp` and `offboard_demo.launch.py`.
Feel free to uncomment it and recompile the package to unveil it.
