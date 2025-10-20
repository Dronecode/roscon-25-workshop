# Precision Land

Precision land package offers a `precision_land` node which implements precision land of PX4 drones through the the _custom modes_ of PX4 ROS 2 interface.

The node subscribes to the topic `/target_pose` (`geometry_msgs::msg::PoseStamped`) representing the pose of the target measured by a down facing camera (hence, the pose is in the camera optical frame).
The node assumes the camera is mounted such that

```math
F_{cam,opt} =
\begin{bmatrix}
0 & 1 & 0 & 0 \\
-1 & 0 & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
F_{drone,FRD}
```

where:

- $F_{cam,opt}$ is the camera optical frame and
- $F_{drone,FRD}$ is the drone body frame in FRD (Front, Right, Down) convention.
- The matrix represent a homogeneous transformation.

The drone position and orientation are instead retrieved trough the PX4 ROS 2 Interface Library.

When the custom `PrecisionLand` mode is activated, the drone will perform the following actions:

1. _Search_ of the target.
Search is implemented executing a spiraling search around the current position.
2. _Approach_ to the target.
When the target is found, the drone moves above it.
3. _Descend_ on the target.
Once it is above the target, the drone will descend.

## How to use

1. Repeat all step made in [aruco_tracker](../aruco_tracker/README)
2. Run the precision land node

    ```sh
    ros2 run precision_land precision_land --ros-args -p use_sim_time:=true
    ```

3. Takeoff and then change into `PrecisionLandCustom` flight mode.

## Exercise

Integrate all the concepts from today. Create a ROS2 package with a Mode Executor for Precision Landing and perform a precision landing in the maze we explored, using teleoperation. Incorporate the custom waypoints you recorded along the way (Create a CustomWaypoints Mode).
Hint: You can start by copying the Custom Mode package. Replace CustomYaw with PrecisionLand and add the waypoints you collected earlier.

The solution can be found in a separate package:
- [Precision Landing with Executor](px4_roscon_25/precision_land_executor/README.md)
