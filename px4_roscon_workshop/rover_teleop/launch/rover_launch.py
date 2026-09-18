
from os import path

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import LaunchConfiguration



def generate_launch_description():

    px4_roscon_workshop_pkg_path = FindPackageShare(package='px4_roscon_workshop').find('px4_roscon_workshop')
    pkg_share = FindPackageShare(package='rover_teleop').find('rover_teleop')

    return LaunchDescription([
        DeclareLaunchArgument(
            'px4_autopilot_path',
            default_value='~/PX4-Autopilot',
            description='Path to PX4-Autopilot repository root (supports ~)',
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                path.join(px4_roscon_workshop_pkg_path, 'launch', 'px4.launch.py')
            ),
            launch_arguments={
                'px4_autopilot_path': LaunchConfiguration('px4_autopilot_path'),
                'world': 'rover',
                'model': 'rover_differential_xl',
                'model_path': path.join(pkg_share, 'models', 'rover_differential_xl', 'model.sdf'),
                'px4_autostart_id': '50000',
                'extra_gz_resource_path': path.join(pkg_share, 'models')
            }.items(),
        ),
        Node(
            package="rover_teleop",
            executable="rover_teleop",
            name="rover_teleop",
            output="screen",
            parameters=[
                {"use_sim_time": True}
            ]
        )
    ])
