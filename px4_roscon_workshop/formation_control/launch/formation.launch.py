from os import path

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import EnvironmentVariable, LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

FORMATION_CONTROL_EXECUTABLE = (
    "px4_formation_control",
    "px4_formation_control_executor_exercise",
    "px4_formation_control_executor_solution",
)
ACTIVE_EXECUTABLE_IDX = 2


def generate_launch_description() -> LaunchDescription:
    px4_roscon_workshop_share = FindPackageShare("px4_roscon_workshop").find(
        "px4_roscon_workshop"
    )
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
                description="Name of the Gazebo world to launch",
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(gz_world_launch),
                launch_arguments={
                    "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
                    "world": LaunchConfiguration("world"),
                }.items(),
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(px4_vehicle_launch),
                launch_arguments={
                    "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
                    "world": LaunchConfiguration("world"),
                    "px4_instance": "0",
                    "model": "x500_0",
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
                    "px4_instance": "1",
                    "model": "x500_1",
                    "px4_ns": "px4_1",
                    "spawn_pos_x": "10.0",
                    "spawn_pos_y": "0.0",
                    "spawn_pos_z": "0.3",
                    "px4_extra_env_vars": "PX4_PARAM_COM_RCL_EXCEPT=9,PX4_PARAM_COM_RC_IN_MODE=4,PX4_GZ_NO_FOLLOW=1",
                }.items(),
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(px4_vehicle_launch),
                launch_arguments={
                    "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
                    "world": LaunchConfiguration("world"),
                    "px4_instance": "2",
                    "model": "x500_2",
                    "px4_ns": "px4_2",
                    "spawn_pos_x": "5.0",
                    "spawn_pos_y": "8.660254",
                    "spawn_pos_z": "0.3",
                    "px4_extra_env_vars": "PX4_PARAM_COM_RCL_EXCEPT=9,PX4_PARAM_COM_RC_IN_MODE=4,PX4_GZ_NO_FOLLOW=1",
                }.items(),
            ),
            Node(
                package="px4_formation_control",
                executable=FORMATION_CONTROL_EXECUTABLE[ACTIVE_EXECUTABLE_IDX],
                name="formation_controller_0",
                output="screen",
                parameters=[
                    {
                        "px4_ns": "/px4_0/",
                        "tf_prefix": "px4_0",
                        "neighbor_prefixes": ["px4_1", "px4_2"],
                        "neighbor_distances": [10.0, 10.0],
                        "gain": 1.0,
                    }
                ],
            ),
            Node(
                package="px4_formation_control",
                executable=FORMATION_CONTROL_EXECUTABLE[ACTIVE_EXECUTABLE_IDX],
                name="formation_controller_1",
                output="screen",
                parameters=[
                    {
                        "px4_ns": "/px4_1/",
                        "tf_prefix": "px4_1",
                        "neighbor_prefixes": ["px4_0", "px4_2"],
                        "neighbor_distances": [10.0, 10.0],
                        "gain": 1.0,
                    }
                ],
            ),
            Node(
                package="px4_formation_control",
                executable=FORMATION_CONTROL_EXECUTABLE[ACTIVE_EXECUTABLE_IDX],
                name="formation_controller_2",
                output="screen",
                parameters=[
                    {
                        "px4_ns": "/px4_2/",
                        "tf_prefix": "px4_2",
                        "neighbor_prefixes": ["px4_0", "px4_1"],
                        "neighbor_distances": [10.0, 10.0],
                        "gain": 1.0,
                    }
                ],
            ),
        ]
    )
