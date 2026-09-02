from os import path
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import (
    EnvironmentVariable,
    LaunchConfiguration,
    PathJoinSubstitution,
)
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    pkg_share = FindPackageShare("px4_roscon_workshop").find("px4_roscon_workshop")

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
                default_value="default",
                description="Name of the world to launch",
            ),
            DeclareLaunchArgument(
                "model",
                default_value="x500",
                description="Name of the model to spawn",
            ),
            DeclareLaunchArgument(
                "model_path",
                default_value=PathJoinSubstitution(
                    [
                        LaunchConfiguration("px4_autopilot_path"),
                        "Tools",
                        "simulation",
                        "gz",
                        "models",
                        "x500",
                        "model.sdf",
                    ]
                ),
                description="Path to the model SDF file",
            ),
            DeclareLaunchArgument(
                "px4_autostart_id",
                default_value="4001",
                description="PX4 autostart ID",
            ),
            DeclareLaunchArgument(
                "extra_gz_resource_path",
                default_value="",
                description="Extra GZ resource path to add to GZ_SIM_RESOURCE_PATH",
            ),
            DeclareLaunchArgument(
                "bridge_config_file",
                default_value=PathJoinSubstitution(
                    [
                        FindPackageShare("px4_roscon_workshop"),
                        "cfg",
                        "clock_bridge.yaml",
                    ]
                ),
                description="Path to the ROS-GZ bridge configuration file",
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(
                    path.join(pkg_share, "launch", "gz_world.launch.py")
                ),
                launch_arguments={
                    "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
                    "world": LaunchConfiguration("world"),
                    "extra_gz_resource_path": LaunchConfiguration(
                        "extra_gz_resource_path"
                    ),
                    "bridge_config_file": LaunchConfiguration("bridge_config_file"),
                }.items(),
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(
                    path.join(pkg_share, "launch", "px4_vehicle.launch.py")
                ),
                launch_arguments={
                    "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
                    "world": LaunchConfiguration("world"),
                    "model": LaunchConfiguration("model"),
                    "model_path": LaunchConfiguration("model_path"),
                    "px4_autostart_id": LaunchConfiguration("px4_autostart_id"),
                }.items(),
            ),
        ]
    )
