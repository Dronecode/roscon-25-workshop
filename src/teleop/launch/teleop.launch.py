from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    config = os.path.join(
        get_package_share_directory('teleop'),
        'cfg',
        'params.yaml'
    )
    return LaunchDescription([
        Node(
            package='teleop',
            executable='teleop',
            name='teleop',
            output='screen',
            parameters=[config],
        )
    ])
