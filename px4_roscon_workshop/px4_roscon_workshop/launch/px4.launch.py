import glob
from os import path, environ
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument, SetEnvironmentVariable, IncludeLaunchDescription, ExecuteProcess, OpaqueFunction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.substitutions import FindPackageShare

# launch gazebo server and PX4
# gz gui needs to be launched separately, e.g.: gz sim -g

def _launch_setup(context):
    px4_autopilot_path = path.expanduser(
        LaunchConfiguration('px4_autopilot_path').perform(context)
    )
    world_name = LaunchConfiguration('world').perform(context)
    model_name = LaunchConfiguration('model').perform(context)
    model_path = path.expanduser(
        LaunchConfiguration('model_path').perform(context)
    )

    extra_gz_resource_path = LaunchConfiguration('extra_gz_resource_path').perform(context)

    ros_gz_sim_pkg_path = get_package_share_directory('ros_gz_sim')
    px4_roscon_workshop_pkg_path = get_package_share_directory('px4_roscon_workshop')
    px4_gz_resource_path = path.join(px4_autopilot_path, 'Tools', 'simulation', 'gz')
    px4_gz_plugin_path = path.join(px4_autopilot_path, 'build', 'px4_sitl_default', 'src', 'modules', 'simulation', 'gz_plugins')
    px4_gz_server_config_path = path.join(px4_autopilot_path, 'src', 'modules', 'simulation', 'gz_bridge', 'server.config')
    gz_launch_path = path.join(ros_gz_sim_pkg_path, 'launch', 'ros_gz_sim.launch.py')
    gz_spawn_path = path.join(ros_gz_sim_pkg_path, 'launch', 'gz_spawn_model.launch.py')

    return [
        SetEnvironmentVariable(
            'GZ_SIM_RESOURCE_PATH',
            ':'.join([path.join(px4_gz_resource_path, 'worlds'), path.join(px4_gz_resource_path, 'models'), extra_gz_resource_path])
        ),
        SetEnvironmentVariable(
            'GZ_SIM_SYSTEM_PLUGIN_PATH',
            ':'.join([environ.get('GZ_SIM_SYSTEM_PLUGIN_PATH', ''), px4_gz_plugin_path])
        ),
        SetEnvironmentVariable(
            'GZ_SIM_SERVER_CONFIG_PATH',
            px4_gz_server_config_path
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gz_launch_path),
            launch_arguments={
                'world_sdf_file': f'{world_name}.sdf',
                'bridge_name': 'gz_ros_bridge',
                'config_file': path.join(px4_roscon_workshop_pkg_path, 'cfg', 'clock_bridge.yaml')
            }.items(),
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gz_spawn_path),
            launch_arguments={
                'world': world_name,
                'file': model_path,
            }.items(),
        ),
        ExecuteProcess(
            cmd = [
                path.join(px4_autopilot_path, 'build', 'px4_sitl_default', 'bin', 'px4'),
            ],
            additional_env = {
                'PX4_SYS_AUTOSTART': LaunchConfiguration('px4_autostart_id').perform(context),
                'PX4_GZ_MODEL_NAME': model_name,
                'PX4_GZ_STANDALONE': '1',
                'PX4_GZ_WORLD': world_name,
            },
            name = 'px4',
            output = 'screen'
        ),
        ExecuteProcess(
            cmd = [
                "MicroXRCEAgent",
                "udp4",
                "--port", "8888",
            ],
            name = 'microxrce_agent',
            output = 'screen'
        )
    ]


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument(
            'px4_autopilot_path',
            default_value='~/PX4-Autopilot',
            description='Path to PX4-Autopilot repository root (supports ~)',
        ),
        DeclareLaunchArgument(
            'world',
            default_value='default',
            description='Name of the world to launch',
        ),
        DeclareLaunchArgument(
            'model',
            default_value='x500',
            description='Name of the model to spawn',
        ),
        DeclareLaunchArgument(
            'model_path',
            default_value='~/PX4-Autopilot/Tools/simulation/gz/models/x500/model.sdf',
            description='Path to the model SDF file',
        ),
        DeclareLaunchArgument(
            'px4_autostart_id',
            default_value='4001',
            description='PX4 autostart ID',
        ),
        DeclareLaunchArgument(
            'extra_gz_resource_path',
            default_value='',
            description='Extra GZ resource path to add to GZ_SIM_RESOURCE_PATH',
        ),
        OpaqueFunction(function=_launch_setup),
    ])