import os

from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node, ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():

    pkg_share = FindPackageShare('px4_roscon_25').find('px4_roscon_25')

    clock_bridge_config_file = os.path.join(pkg_share,"cfg","clock_bridge.yaml")

    urdf_dir = os.path.join(pkg_share, 'urdf')
    urdf_file = os.path.join(urdf_dir, 'x500.urdf')
    with open(urdf_file, 'r') as infp:
        x500_desc = infp.read()

    return LaunchDescription([
        Node(
            package="ros_gz_bridge",
            executable="parameter_bridge",
            name="gz_clock_bridge",
            parameters=[
                {"config_file": clock_bridge_config_file}
            ]
        ),
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            output='both',
            parameters=[
                {
                'robot_description': x500_desc,
                'use_sim_time': True
                }
            ]
        ),
        Node(
            package='px4_tf',
            executable='px4_tf_publisher',
            parameters=[
                {
                'use_sim_time': True
                }
            ]
        ),
        Node(
            package='foxglove_bridge',
            executable='foxglove_bridge',
            parameters=[
                {'use_sim_time': True}
            ],
        ),
        ComposableNodeContainer(
            name='static_tf_container',
            package='rclcpp_components',
            executable='component_container',
            namespace='',
            composable_node_descriptions=[
                ComposableNode(
                    package='tf2_ros',
                    plugin='tf2_ros::StaticTransformBroadcasterNode',
                    name='map_to_odom_broadcaster',
                    parameters=[{
                        'use_sim_time': True,
                        'translation.x': 0.0,
                        'translation.y': 0.0,
                        'translation.z': 0.24,
                        'rotation.x': 0.0,
                        'rotation.y': 0.0,
                        'rotation.z': 0.0,
                        'frame_id': 'map',
                        'child_frame_id': 'odom'
                    }]
                ),
                # other static transform nodes
            ]
        ),
        ExecuteProcess(
            cmd=["MicroXRCEAgent", "udp4", "-p", "8888", "-v", "3"],
            output="screen",
        )
    ])