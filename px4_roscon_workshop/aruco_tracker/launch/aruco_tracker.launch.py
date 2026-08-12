import os

import yaml
import tempfile

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import LoadComposableNodes, Node
from launch_ros.substitutions import FindPackageShare
from launch_ros.descriptions import ComposableNode

def launch_setup(context, *args, **kwargs):
    return_array = []

    world_name = LaunchConfiguration("world_name").perform(context)
    model_name = LaunchConfiguration("model_name").perform(context)

    camera_topic = f"/world/{world_name}/model/{model_name}/link/camera_link/sensor/imager/image"
    camera_info_topic = f"/world/{world_name}/model/{model_name}/link/camera_link/sensor/imager/camera_info"

    pkg_share = FindPackageShare("aruco_tracker").find("aruco_tracker")

    bridge_config_file = os.path.join(pkg_share,"cfg","bridge.yaml")
    aruco_tracker_config_file = os.path.join(pkg_share, 'cfg', 'params.yaml')

    with open(bridge_config_file, 'r') as file:
        bridge_config = yaml.safe_load(file)

    for item in bridge_config:
        item['gz_topic_name'] = item['gz_topic_name'].format(
            world_name=world_name,
            model_name=model_name,
        )

    tmp_file = tempfile.NamedTemporaryFile(suffix=".yaml", delete=False)
    with open(tmp_file.name, 'w') as file:
        yaml.dump(bridge_config, file)

    return_array.append(
        Node(
            package="ros_gz_bridge",
            executable="parameter_bridge",
            name="gz_bridge",
            parameters=[
                {"config_file": tmp_file.name}
            ],
        )
    )
    return_array.append(
        Node(
            package="aruco_tracker",
            executable="aruco_tracker",
            name="aruco_tracker",
            output="screen",
            parameters=[
                {"use_sim_time": True},
                aruco_tracker_config_file
            ]
        )
    )
    return_array.append(
        Node(
            package="ros_gz_image",
            executable="image_bridge",
            name="camera_image_bridge",
            output="screen",
            arguments=[
                camera_topic,
            ],
            remappings=[
                (camera_topic, "/camera"),
                (f"{camera_topic}/compressed", "/camera/compressed"),
            ],
        )
    )
    return_array.append(
        LoadComposableNodes(
            target_container='static_tf_container',
            composable_node_descriptions=[
                ComposableNode(
                    package='tf2_ros',
                    plugin='tf2_ros::StaticTransformBroadcasterNode',
                    name='base_link_to_camera_broadcaster',
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
                    name='camera_to_camera_frame_broadcaster',
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
    )
    return return_array

def generate_launch_description():

    return LaunchDescription([
        DeclareLaunchArgument(
            "world_name",
            default_value="aruco",
            description="name of the Gazebo world",
        ),
        DeclareLaunchArgument(
            "model_name",
            default_value="x500_mono_cam_down_0",
            description="name of the model",
        ),
        OpaqueFunction(
            function=launch_setup,
        ),
    ])