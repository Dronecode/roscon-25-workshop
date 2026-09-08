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
from launch_ros.actions import Node, SetParameter


def generate_launch_description() -> LaunchDescription:
    pkg_share = FindPackageShare("px4_gimbal_control").find("px4_gimbal_control")
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

    camera_topic = (
        "/world/rover/model/vtol_gimbal_0/link/camera_link/sensor/camera/image"
    )

    return LaunchDescription(
        [
            SetParameter(name="use_sim_time", value=True),
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
                    "bridge_config_file": path.join(pkg_share, "cfg", "bridge.yaml"),
                    "extra_gz_resource_path": path.join(
                        px4_roscon_workshop_share, "models"
                    ),
                }.items(),
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(px4_vehicle_launch),
                launch_arguments={
                    "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
                    "world": LaunchConfiguration("world"),
                    "model_path": PathJoinSubstitution(
                        [
                            px4_roscon_workshop_share,
                            "models",
                            "vtol_gimbal",
                            "model.sdf",
                        ]
                    ),
                    "px4_instance": "0",
                    "px4_autostart_id": "4004",
                    "model": "vtol_gimbal_0",
                    "spawn_pos_x": "0.0",
                    "spawn_pos_y": "0.0",
                    "spawn_pos_z": "0.3",
                    "px4_extra_env_vars": ",".join(
                        [
                            "PX4_PARAM_COM_RCL_EXCEPT=9",
                            "PX4_PARAM_COM_RC_IN_MODE=1",
                            "PX4_GZ_NO_FOLLOW=1",
                            "PX4_PARAM_MNT_MAN_ROLL=1",
                            "PX4_PARAM_MNT_MAN_PITCH=2",
                            "PX4_PARAM_MNT_MAN_YAW=3",
                            "PX4_PARAM_MNT_RANGE_ROLL=180",
                            "PX4_PARAM_MNT_MAX_PITCH=45",
                            "PX4_PARAM_MNT_MIN_PITCH=-135",
                            "PX4_PARAM_MNT_RANGE_YAW=720",
                            "PX4_PARAM_MNT_MODE_OUT=2",
                        ]
                    ),
                }.items(),
            ),
            Node(
                package="ros_gz_image",
                executable="image_bridge",
                name="camera_image_bridge",
                output="screen",
                arguments=[
                    camera_topic,
                ],
                remappings=[
                    (camera_topic, "/camera"),
                    (f"{camera_topic}/compressed", "/camera/compressed"),
                ],
            ),
            Node(
                package="px4_gimbal_control",
                executable="gimbal_teleop",
                name="gimbal_teleop",
                output="screen",
                parameters=[
                    {"yaw_limit": 270.0, "pitch_min": -135.0, "pitch_max": 45.0}
                ],
            ),
            Node(
                package="px4_gimbal_control",
                executable="mouse_pointer_gimbal_control",
                name="mouse_pointer_gimbal_control",
                output="screen",
            ),
        ]
    )


# ros2 topic pub -r 5 /px4_0/fmu/in/gimbal_controls px4_msgs/msg/GimbalControls "timestamp: 0
# timestamp_sample: 0
# control:
# - 0.0
# - 0.0
# - 0.0
# "
