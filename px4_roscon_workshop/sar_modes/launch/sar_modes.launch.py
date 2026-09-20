from os import path

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import EnvironmentVariable, LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from launch_ros.substitutions import FindPackageShare

# Per-drone Gazebo spawn positions (ENU, spread apart so they don't overlap).
DRONE_SPAWN_POSITIONS = [(0.0, 0.0), (2.0, 0.0), (4.0, 0.0)]


def _float_param(launch_arg_name):
    return ParameterValue(LaunchConfiguration(launch_arg_name), value_type=float)


def generate_launch_description():

    px4_autopilot_path_arg = DeclareLaunchArgument(
        "px4_autopilot_path",
        default_value=EnvironmentVariable("PX4_PATH", default_value="~/PX4-Autopilot"),
        description="Path to PX4-Autopilot repository root (supports ~)",
    )
    world_arg = DeclareLaunchArgument(
        "world", default_value="default",
        description="Name of the Gazebo world to launch"
    )

    rover_x_arg = DeclareLaunchArgument(
        "rover_start_x", default_value="5.0",
        description="Fake rover starting ENU X position [m]"
    )
    rover_y_arg = DeclareLaunchArgument(
        "rover_start_y", default_value="3.0",
        description="Fake rover starting ENU Y position [m]"
    )
    rover_yaw_arg = DeclareLaunchArgument(
        "rover_start_yaw_deg", default_value="45.0",
        description="Fake rover starting heading [deg]"
    )
    rover_min_speed_arg = DeclareLaunchArgument(
        "rover_min_linear_speed", default_value="0.2",
        description="Fake rover linear speed during the straight phase [m/s]"
    )
    rover_max_speed_arg = DeclareLaunchArgument(
        "rover_max_linear_speed", default_value="0.8",
        description="Fake rover linear speed during the circle phase [m/s]"
    )
    rover_start_delay_arg = DeclareLaunchArgument(
        "rover_start_delay", default_value="3.0",
        description="Seconds the fake rover stays still before it starts moving"
    )
    rover_straight_duration_arg = DeclareLaunchArgument(
        "rover_straight_duration", default_value="45.0",
        description="Seconds the fake rover drives straight before circling [s]"
    )
    rover_circle_radius_arg = DeclareLaunchArgument(
        "rover_circle_radius", default_value="5.0",
        description="Fake rover circling radius, minimum 5m [m]"
    )
    rover_num_circle_turns_arg = DeclareLaunchArgument(
        "rover_num_circle_turns", default_value="2.0",
        description="Number of full turns the fake rover circles before holding"
    )

    fake_rover_pose = Node(
        package="sar_modes",
        executable="fake_rover_mover.py",
        name="fake_rover_mover",
        output="screen",
        parameters=[
            {
                "start_x": _float_param("rover_start_x"),
                "start_y": _float_param("rover_start_y"),
                "start_yaw_deg": _float_param("rover_start_yaw_deg"),
                "min_linear_speed": _float_param("rover_min_linear_speed"),
                "max_linear_speed": _float_param("rover_max_linear_speed"),
                "start_delay": _float_param("rover_start_delay"),
                "straight_duration": _float_param("rover_straight_duration"),
                "circle_radius": _float_param("rover_circle_radius"),
                "num_circle_turns": _float_param("rover_num_circle_turns"),
            }
        ]
    )

    total_drones = 3

    px4_roscon_workshop_share = FindPackageShare("px4_roscon_workshop").find(
        "px4_roscon_workshop"
    )
    gz_world_launch = path.join(px4_roscon_workshop_share, "launch", "gz_world.launch.py")
    px4_vehicle_launch = path.join(px4_roscon_workshop_share, "launch", "px4_vehicle.launch.py")

    gz_world = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_world_launch),
        launch_arguments={
            "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
            "world": LaunchConfiguration("world"),
        }.items(),
    )

    # PX4 multi-vehicle instances are 1-indexed in this repo's convention
    # (see px4_roscon_workshop/px4_tf/README.md: -i 1 -> /px4_1/..., -i 2 -> /px4_2/...),
    # but the formation math in SARMode.cpp centers around a 0-indexed drone_id.
    # So the ROS namespace uses the PX4 instance id, while the drone_id
    # parameter passed to the node stays 0-indexed.
    px4_vehicles = [
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(px4_vehicle_launch),
            launch_arguments={
                "px4_autopilot_path": LaunchConfiguration("px4_autopilot_path"),
                "world": LaunchConfiguration("world"),
                "px4_instance": str(instance_id),
                "model": f"x500_{instance_id}",
                "px4_ns": f"px4_{instance_id}",
                "spawn_pos_x": str(DRONE_SPAWN_POSITIONS[instance_id - 1][0]),
                "spawn_pos_y": str(DRONE_SPAWN_POSITIONS[instance_id - 1][1]),
                "px4_extra_env_vars": (
                    "PX4_PARAM_COM_RCL_EXCEPT=9,"
                    f"PX4_PARAM_COM_RC_IN_MODE={1 if instance_id == 1 else 4},"
                    "PX4_GZ_NO_FOLLOW=1"
                ),
            }.items(),
        )
        for instance_id in range(1, total_drones + 1)
    ]

    drone_nodes = [
        Node(
            package="sar_modes",
            executable="sar_modes",
            name="sar_modes",
            namespace=f"px4_{instance_id}",
            output="screen",
            parameters=[
                {
                    "use_sim_time": True,
                    "drone_id": instance_id - 1,
                    "total_drones": total_drones,
                }
            ]
        )
        for instance_id in range(1, total_drones + 1)
    ]

    return LaunchDescription([
        px4_autopilot_path_arg,
        world_arg,
        rover_x_arg,
        rover_y_arg,
        rover_yaw_arg,
        rover_min_speed_arg,
        rover_max_speed_arg,
        rover_start_delay_arg,
        rover_straight_duration_arg,
        rover_circle_radius_arg,
        rover_num_circle_turns_arg,
        gz_world,
        *px4_vehicles,
        *drone_nodes,
        fake_rover_pose,
    ])
