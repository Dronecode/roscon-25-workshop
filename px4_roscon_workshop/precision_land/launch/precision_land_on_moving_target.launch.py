from os import path

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import (
    EnvironmentVariable,
    LaunchConfiguration,
    PathJoinSubstitution,
)
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description() -> LaunchDescription:
    px4_roscon_workshop_share = FindPackageShare("px4_roscon_workshop").find(
        "px4_roscon_workshop"
    )
    aruco_tracker_share = FindPackageShare("aruco_tracker").find("aruco_tracker")
    gz_world_launch = path.join(
        px4_roscon_workshop_share,
        "launch",
        "gz_world.launch.py",
    )
    px4_vehicle_launch = path.join(
        px4_roscon_workshop_share,
        "launch",
        "px4_vehicle.launch.py",
    )
    aruco_tracker_launch = path.join(
        aruco_tracker_share,
        "launch",
        "aruco_tracker.launch.py",
    )
    rover_pkg_share = FindPackageShare(package="rover_teleop").find("rover_teleop")

    return LaunchDescription(
        [
            DeclareLaunchArgument(
                "px4_autopilot_path",
                default_value=EnvironmentVariable(
                    "PX4_PATH", default_value="~/PX4-Autopilot"
                ),
                description="Path to PX4-Autopilot repository root (supports ~)",
            ),
            DeclareLaunchArgument(
                "world",
                default_value="rover",
                description="Name of the Gazebo world to launch",
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(gz_world_launch),
                launch_arguments={
                    "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
                    "world": LaunchConfiguration("world"),
                    "extra_gz_resource_path": path.join(rover_pkg_share, "models"),
                }.items(),
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(px4_vehicle_launch),
                launch_arguments={
                    "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
                    "world": LaunchConfiguration("world"),
                    "model_path": PathJoinSubstitution(
                        [
                            LaunchConfiguration("px4_autopilot_path"),
                            "Tools",
                            "simulation",
                            "gz",
                            "models",
                            "x500_mono_cam_down",
                            "model.sdf",
                        ]
                    ),
                    "px4_instance": "0",
                    "px4_autostart_id": "4014",
                    "model": "x500_mono_cam_down_0",
                    "px4_ns": "px4_0",
                    "spawn_pos_x": "0.0",
                    "spawn_pos_y": "0.0",
                    "spawn_pos_z": "0.3",
                    "px4_extra_env_vars": "PX4_PARAM_COM_RCL_EXCEPT=9,PX4_PARAM_COM_RC_IN_MODE=1,PX4_GZ_NO_FOLLOW=1",
                }.items(),
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(px4_vehicle_launch),
                launch_arguments={
                    "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
                    "world": LaunchConfiguration("world"),
                    "model_path": path.join(
                        rover_pkg_share, "models", "rover_differential_xl", "model.sdf"
                    ),
                    "px4_instance": "1",
                    "px4_autostart_id": "50000",
                    "model": "rover_differential_xl_1",
                    "px4_ns": "px4_1",
                    "spawn_pos_x": "5.0",
                    "spawn_pos_y": "0.0",
                    "spawn_pos_z": "0.0",
                    "px4_extra_env_vars": "PX4_PARAM_COM_RCL_EXCEPT=9,PX4_PARAM_COM_RC_IN_MODE=1,PX4_GZ_NO_FOLLOW=1",
                }.items(),
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(aruco_tracker_launch),
                launch_arguments={
                    "world_name": LaunchConfiguration("world"),
                    "model_name": "x500_mono_cam_down_0",
                }.items(),
            ),
            Node(
                package="precision_land",
                executable="precision_land",
                name="precision_land_node",
                output="screen",
                namespace="px4_0",
            ),
            Node(
                package="rover_teleop",
                executable="rover_teleop",
                name="rover_teleop_node",
                output="screen",
                namespace="px4_1",
            ),
        ]
    )
