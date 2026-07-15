from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description() -> LaunchDescription:
    return LaunchDescription([
        Node(
            package='px4_formation_control',
            executable='px4_formation_control',
            name='formation_controller_1',
            output='screen',
            parameters=[{
                'px4_ns': "/px4_1/",
                'tf_prefix': "px4_1",
                'neighbor_prefixes': ['px4_2', 'px4_3'],
                'neighbor_distances': [10.0, 10.0],
                'gain': 1.0
            }]
        ),
        Node(
            package='px4_formation_control',
            executable='px4_formation_control',
            name='formation_controller_2',
            output='screen',
            parameters=[{
                'px4_ns': "/px4_2/",
                'tf_prefix': "px4_2",
                'neighbor_prefixes': ['px4_1', 'px4_3'],
                'neighbor_distances': [10.0, 10.0],
                'gain': 1.0
            }]
        ),
        Node(
            package='px4_formation_control',
            executable='px4_formation_control',
            name='formation_controller_3',
            output='screen',
            parameters=[{
                'px4_ns': "/px4_3/",
                'tf_prefix': "px4_3",
                'neighbor_prefixes': ['px4_1', 'px4_2'],
                'neighbor_distances': [10.0, 10.0],
                'gain': 1.0
            }]
        )
    ])
