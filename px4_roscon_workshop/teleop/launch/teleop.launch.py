import os
from launch import LaunchDescription
from launch.actions import ExecuteProcess, DeclareLaunchArgument, OpaqueFunction
from launch_ros.actions import LoadComposableNodes, Node
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import LaunchConfiguration
from launch_ros.descriptions import ComposableNode

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

    pkg_share = FindPackageShare("teleop").find("teleop")

    bridge_config_file = os.path.join(pkg_share, "cfg", 'bridge.yaml')

    return LaunchDescription([
        DeclareLaunchArgument(
            'px4_autopilot_path',
            default_value='~/PX4-Autopilot',
            description='Path to PX4-Autopilot repository root (supports ~)',
        ),
        Node(
            package="ros_gz_bridge",
            executable="parameter_bridge",
            name="gz_pointcloud_bridge",
            parameters=[
                {"config_file": bridge_config_file}
            ]
        ),
        Node(
            package="teleop",
            executable="teleop",
            name="teleop",
            output="screen",
            parameters=[
                {"use_sim_time": True}
            ]
        ),
        LoadComposableNodes(
            target_container='static_tf_container',
            composable_node_descriptions=[
                ComposableNode(
                    package='tf2_ros',
                    plugin='tf2_ros::StaticTransformBroadcasterNode',
                    name='base_link_to_link_broadcaster',
                    parameters=[{
                        'use_sim_time': True,
                        'translation.x': 0.0,
                        'translation.y': 0.0,
                        'translation.z': 0.0,
                        'rotation.x': 0.0,
                        'rotation.y': 0.0,
                        'rotation.z': 0.0,
                        'rotation.w': 1.0,
                        'frame_id': 'base_link',
                        'child_frame_id': 'link'
                    }]
                ),
                ComposableNode(
                    package='tf2_ros',
                    plugin='tf2_ros::StaticTransformBroadcasterNode',
                    name='map_to_target_aruco_broadcaster',
                    parameters=[{
                        'use_sim_time': True,
                        'translation.x': 8.0,
                        'translation.y': -4.0,
                        'translation.z': 0.001,
                        'rotation.x': 0.0,
                        'rotation.y': 0.0,
                        'rotation.z': 0.0,
                        'rotation.w': 1.0,
                        'frame_id': 'map',
                        'child_frame_id': 'aruco_target_gt'
                    }]
                ),
            ]
        ),
        OpaqueFunction(function=_launch_setup)
    ])