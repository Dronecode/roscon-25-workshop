import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    pkg_share = get_package_share_directory("aruco_tracker")

    bridge_config_file = os.path.join(pkg_share,"cfg","bridge.yaml")
    aruco_tracker_config_file = os.path.join(pkg_share, 'cfg', 'params.yaml')

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
        ExecuteProcess(
            cmd=["MicroXRCEAgent", "udp4", "-p", "8888", "-v", "3"],
            output="screen",
            condition=IfCondition(LaunchConfiguration("run_uxrcedds_agent"))
        )
    ])