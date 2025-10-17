import os

from launch import LaunchDescription
from launch_ros.actions import LoadComposableNodes, Node
from launch_ros.substitutions import FindPackageShare
from launch_ros.descriptions import ComposableNode

def generate_launch_description():

    pkg_share = FindPackageShare("teleop").find("teleop")

    bridge_config_file = os.path.join(pkg_share, "cfg", 'bridge.yaml')

    return LaunchDescription([
        Node(
            package="ros_gz_bridge",
            executable="parameter_bridge",
            name="gz_clock_bridge",
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
                    name='map_to_odom_broadcaster',
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
                        'child_frame_id': 'x500_lidar_2d_0/link/lidar_2d_v2'
                    }]
                ),
            ]
        ),
    ])