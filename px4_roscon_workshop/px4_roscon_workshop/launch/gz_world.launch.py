from os import environ, path

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    ExecuteProcess,
    IncludeLaunchDescription,
    OpaqueFunction,
    SetEnvironmentVariable,
)
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import EnvironmentVariable, LaunchConfiguration, PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare


def _launch_setup(context):
    px4_autopilot_path = path.expanduser(
        LaunchConfiguration('px4_autopilot_path').perform(context)
    )
    world_name = LaunchConfiguration('world').perform(context)
    extra_gz_resource_path = LaunchConfiguration('extra_gz_resource_path').perform(context)
    bridge_config_file = LaunchConfiguration('bridge_config_file').perform(context)

    ros_gz_sim_pkg_path = get_package_share_directory('ros_gz_sim')
    px4_gz_resource_path = path.join(px4_autopilot_path, 'Tools', 'simulation', 'gz')
    px4_gz_plugin_path = path.join(
        px4_autopilot_path,
        'build',
        'px4_sitl_default',
        'src',
        'modules',
        'simulation',
        'gz_plugins',
    )
    px4_gz_server_config_path = path.join(
        px4_autopilot_path,
        'src',
        'modules',
        'simulation',
        'gz_bridge',
        'server.config',
    )
    gz_launch_path = path.join(ros_gz_sim_pkg_path, 'launch', 'ros_gz_sim.launch.py')

    return [
        SetEnvironmentVariable(
            'GZ_SIM_RESOURCE_PATH',
            ':'.join([
                path.join(px4_gz_resource_path, 'worlds'),
                path.join(px4_gz_resource_path, 'models'),
                extra_gz_resource_path,
            ]),
        ),
        SetEnvironmentVariable(
            'GZ_SIM_SYSTEM_PLUGIN_PATH',
            ':'.join([environ.get('GZ_SIM_SYSTEM_PLUGIN_PATH', ''), px4_gz_plugin_path]),
        ),
        SetEnvironmentVariable('GZ_SIM_SERVER_CONFIG_PATH', px4_gz_server_config_path),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gz_launch_path),
            launch_arguments={
                'world_sdf_file': f'{world_name}.sdf',
                'bridge_name': 'gz_ros_bridge',
                'config_file': bridge_config_file,
            }.items(),
        ),
        ExecuteProcess(
            cmd=['MicroXRCEAgent', 'udp4', '--port', '8888', '-v', '1'],
            name='microxrce_agent',
            output='screen',
        ),
    ]


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument(
            'px4_autopilot_path',
            default_value=EnvironmentVariable('PX4_PATH', default_value='~/PX4-Autopilot'),
            description='Path to PX4-Autopilot repository root (supports ~)',
        ),
        DeclareLaunchArgument(
            'world',
            default_value='default',
            description='Name of the world to launch',
        ),
        DeclareLaunchArgument(
            'extra_gz_resource_path',
            default_value='',
            description='Extra GZ resource path to add to GZ_SIM_RESOURCE_PATH',
        ),
        DeclareLaunchArgument(
            'bridge_config_file',
            default_value=PathJoinSubstitution([
                FindPackageShare('px4_roscon_workshop'),
                'cfg',
                'clock_bridge.yaml',
            ]),
            description='Path to the ROS-GZ bridge configuration file',
        ),
        OpaqueFunction(function=_launch_setup),
    ])