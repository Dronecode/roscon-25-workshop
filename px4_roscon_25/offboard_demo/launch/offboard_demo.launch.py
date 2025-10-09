import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    pkg_share = get_package_share_directory("offboard_demo")
    
    clock_bridge_config_file = os.path.join(pkg_share,"cfg","clock_bridge.yaml")

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
            package="offboard_demo",
            executable="offboard_demo",
            name="offboard_demo",
            output="screen",
            parameters=[
                {"use_sim_time": True},
                {"takeoff_altitude": 3.0}
            ]
        ),
        ExecuteProcess(
            cmd=["MicroXRCEAgent", "udp4", "-p", "8888", "-v", "3"],
            output="screen",
            condition=IfCondition(LaunchConfiguration("run_uxrcedds_agent"))
        )
    ])
