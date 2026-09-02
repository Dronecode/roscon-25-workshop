import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    pkg_share = get_package_share_directory("vtol_executor")

    params_file = os.path.join(pkg_share, "cfg", "params.yaml")

    run_uxrcedds_agent_arg = DeclareLaunchArgument(
        "run_uxrcedds_agent",
        default_value="false",
        description="Whether to run the MicroXRCEdds Agent",
    )

    return LaunchDescription(
        [
            run_uxrcedds_agent_arg,
            Node(
                package="vtol_executor",
                executable="vtol_executor",
                name="vtol_executor",
                output="screen",
                parameters=[params_file, {"use_sim_time": True}],
            ),
            ExecuteProcess(
                cmd=["MicroXRCEAgent", "udp4", "-p", "8888", "-v", "3"],
                output="screen",
                condition=IfCondition(LaunchConfiguration("run_uxrcedds_agent")),
            ),
        ]
    )
