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

import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    pkg_share = get_package_share_directory("teleop")

    clock_bridge_config_file = os.path.join(pkg_share,"cfg","clock_bridge.yaml")
    teleop_config_file = os.path.join(pkg_share, 'cfg', 'params.yaml')

    run_uxrcedds_agent_arg = DeclareLaunchArgument(
        "run_uxrcedds_agent",
        default_value="false",
        description="Whether to run the MicroXRCEdds Agent",
    )

    return LaunchDescription([
        run_uxrcedds_agent_arg,
        Node(
            package="ros_gz_bridge",
            executable="parameter_bridge",
            name="gz_clock_bridge",
            parameters=[
                {"config_file": clock_bridge_config_file}
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
        ExecuteProcess(
            cmd=["MicroXRCEAgent", "udp4", "-p", "8888", "-v", "3"],
            output="screen",
            condition=IfCondition(LaunchConfiguration("run_uxrcedds_agent"))
        )
    ])