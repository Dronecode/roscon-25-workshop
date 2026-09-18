from os import path

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess, IncludeLaunchDescription, OpaqueFunction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import EnvironmentVariable, LaunchConfiguration, PathJoinSubstitution


def _launch_setup(context):
    px4_autopilot_path = path.expanduser(
        LaunchConfiguration('px4_autopilot_path').perform(context)
    )
    world_name = LaunchConfiguration('world').perform(context)
    model_name = LaunchConfiguration('model').perform(context)
    model_path = path.expanduser(
        LaunchConfiguration('model_path').perform(context)
    )
    px4_instance = LaunchConfiguration('px4_instance').perform(context)
    px4_namespace = LaunchConfiguration('px4_ns').perform(context)

    px4_extra_env_vars = LaunchConfiguration('px4_extra_env_vars').perform(context)

    ros_gz_sim_pkg_path = get_package_share_directory('ros_gz_sim')
    gz_spawn_path = path.join(ros_gz_sim_pkg_path, 'launch', 'gz_spawn_model.launch.py')

    px4_env = {
        'PX4_SYS_AUTOSTART': LaunchConfiguration('px4_autostart_id').perform(context),
        'PX4_GZ_MODEL_NAME': model_name,
        'PX4_GZ_STANDALONE': '1',
        'PX4_GZ_WORLD': world_name,
    }
    px4_env.update(
        dict(
            (key.strip(), value.strip())
            for env_var in px4_extra_env_vars.split(',')
            if env_var.strip()
            for key, value in [env_var.split('=', 1)]
        )
    )
    if px4_namespace:
        px4_env['PX4_UXRCE_DDS_NS'] = px4_namespace

    return [
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gz_spawn_path),
            launch_arguments={
                'world': world_name,
                'file': model_path,
                'entity_name': model_name,
                'x': LaunchConfiguration('spawn_pos_x').perform(context),
                'y': LaunchConfiguration('spawn_pos_y').perform(context),
                'z': LaunchConfiguration('spawn_pos_z').perform(context),
            }.items(),
        ),
        ExecuteProcess(
            cmd=[
                path.join(px4_autopilot_path, 'build', 'px4_sitl_default', 'bin', 'px4'),
                '-i', px4_instance,
            ],
            additional_env=px4_env,
            name=f'px4_{px4_instance}',
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
            'px4_instance',
            default_value='0',
            description='PX4 instance ID',
        ),
        DeclareLaunchArgument(
            'world',
            default_value='default',
            description='Name of the world where the model is spawned',
        ),
        DeclareLaunchArgument(
            'model',
            default_value='x500',
            description='Name of the model to start PX4 for',
        ),
        DeclareLaunchArgument(
            'model_path',
            default_value=PathJoinSubstitution([
                LaunchConfiguration('px4_autopilot_path'),
                'Tools',
                'simulation',
                'gz',
                'models',
                'x500',
                'model.sdf',
            ]),
            description='Path to the model SDF file',
        ),
        DeclareLaunchArgument(
            'px4_autostart_id',
            default_value='4001',
            description='PX4 autostart ID',
        ),
        DeclareLaunchArgument(
            'px4_ns',
            default_value='',
            description='PX4 namespace',
        ),
        DeclareLaunchArgument(
            'px4_extra_env_vars',
            default_value='',
            description='Extra environment variables to set for PX4',
        ),
        DeclareLaunchArgument(
            'spawn_pos_x',
            default_value='0.0',
            description='X position to spawn the model at',
        ),
        DeclareLaunchArgument(
            'spawn_pos_y',
            default_value='0.0',
            description='Y position to spawn the model at',
        ),
        DeclareLaunchArgument(
            'spawn_pos_z',
            default_value='0.0',
            description='Z position to spawn the model at',
        ),
        OpaqueFunction(function=_launch_setup),
    ])