import os

from launch import LaunchDescription
from launch.actions import ExecuteProcess, DeclareLaunchArgument, OpaqueFunction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def _launch_setup(context):
    px4_autopilot_path = os.path.expanduser(
        LaunchConfiguration('px4_autopilot_path').perform(context)
    )
    px4_gz_resource_path = os.path.join(px4_autopilot_path, 'Tools', 'simulation', 'gz')
    arucotag_model_path = os.path.join(px4_gz_resource_path, 'models', 'arucotag')
    return [
        # SetEnvironmentVariable(
        #     'GZ_SIM_RESOURCE_PATH',
        #     ':'.join([os.path.join(px4_gz_resource_path, 'worlds'), os.path.join(px4_gz_resource_path, 'models')])
        # ),
        ExecuteProcess(
            cmd=[
                "gz", "service",
                "-s", "/world/walls/create",
                "--reqtype", "gz.msgs.EntityFactory",
                "--reptype", "gz.msgs.Boolean",
                "--timeout", "1000",
                "--req",
                (
                    f'sdf_filename: "{arucotag_model_path}/model.sdf", '
                    'name: "arucotag", '
                    'pose: { position: { x: 8, y: -4.0, z: 0.001000 }, '
                    'orientation: { x: 0.0, y: 0.0, z: 0.0, w: 1.0 } }'
                )
            ],
            output="screen"
        )
    ]

def generate_launch_description():

    return LaunchDescription([
        DeclareLaunchArgument(
            'px4_autopilot_path',
            default_value='~/PX4-Autopilot',
            description='Path to PX4-Autopilot repository root (supports ~)',
        ),
        Node(
            package="precision_land_executor",
            executable="precision_land_executor",
            name="precision_land_executor",
            output="screen",
            parameters=[
                {"use_sim_time": True}
            ]
        ),
        OpaqueFunction(function=_launch_setup),
    ])
