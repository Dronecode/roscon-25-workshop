import os

from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    pkg_share = get_package_share_directory("offboard_demo")
    
    clock_bridge_config_file = os.path.join(pkg_share,"cfg","clock_bridge.yaml")

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
            package="offboard_demo",
            executable="offboard_demo",
            name="offboard_demo",
            output="screen",
            parameters=[
                {"use_sim_time": True}
            ]
        )
    ])
