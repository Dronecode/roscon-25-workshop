import os

from launch import LaunchDescription
from launch_ros.actions import LoadComposableNodes, Node
from launch_ros.substitutions import FindPackageShare
from launch_ros.descriptions import ComposableNode

def generate_launch_description():

    pkg_share = FindPackageShare("aruco_tracker").find("aruco_tracker")

    bridge_config_file = os.path.join(pkg_share,"cfg","bridge.yaml")
    aruco_tracker_config_file = os.path.join(pkg_share, 'cfg', 'params.yaml')

    return LaunchDescription([
        Node(
            package="ros_gz_bridge",
            executable="parameter_bridge",
            name="gz_bridge",
            parameters=[
                {"config_file": bridge_config_file}
            ]
        ),
        Node(
            package="aruco_tracker",
            executable="aruco_tracker",
            name="aruco_tracker",
            output="screen",
            parameters=[
                {"use_sim_time": True},
                aruco_tracker_config_file
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
                        'rotation.x': -0.7071068,
                        'rotation.y': 0.7071068,
                        'rotation.z': 0.0,
                        'rotation.w': 0.0,
                        'frame_id': 'base_link',
                        'child_frame_id': 'x500_mono_cam_down_0/camera_link/imager'
                    }]
                ),
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
                        'rotation.w': 10.0,
                        'frame_id': 'x500_mono_cam_down_0/camera_link/imager',
                        'child_frame_id': 'camera_frame'
                    }]
                ),
            ]
        ),
    ])

    

    # <!--
    # <link name="x500_lidar_2d_mono_cam_down_0/camera_link/imager" />
    # <joint name="base_to_camera" type="fixed">
    #     <parent link="base_link"/>
    #     <child link="x500_lidar_2d_mono_cam_down_0/camera_link/imager"/>
    #     <origin xyz="0 0 0" rpy="0 3.14 1.5707"/>
    # </joint>
    # -->