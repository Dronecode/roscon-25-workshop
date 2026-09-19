import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess, SetEnvironmentVariable
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    pkg_share = get_package_share_directory("custom_mode_demo")

    clock_bridge_config_file = os.path.join(pkg_share, "cfg", "clock_bridge.yaml")

    run_uxrcedds_agent_arg = DeclareLaunchArgument(
        "run_uxrcedds_agent",
        default_value="false",
        description="Whether to run the MicroXRCEdds Agent",
    )

    run_gz_clock_bridge_arg = DeclareLaunchArgument(
        "run_gz_clock_bridge",
        default_value="false",
        description="Whether to run the Gazebo clock bridge",
    )

    return LaunchDescription(
        [
            run_uxrcedds_agent_arg,
            run_gz_clock_bridge_arg,
            # Default gz-transport discovery to loopback (fixes multicast discovery
            # breaking on machines whose default route is Wi-Fi), without
            # overriding an explicit GZ_IP the user may have already set.
            SetEnvironmentVariable("GZ_IP", os.environ.get("GZ_IP", "127.0.0.1")),
            Node(
                package="custom_mode_demo",
                executable="custom_mode_demo",
                name="custom_mode_demo",
                output="screen",
                parameters=[{"use_sim_time": True}],
            ),
            Node(
                package="ros_gz_bridge",
                executable="parameter_bridge",
                name="gz_clock_bridge",
                parameters=[{"config_file": clock_bridge_config_file}],
                condition=IfCondition(LaunchConfiguration("run_gz_clock_bridge")),
            ),
            ExecuteProcess(
                cmd=["MicroXRCEAgent", "udp4", "-p", "8888", "-v", "3"],
                output="screen",
                condition=IfCondition(LaunchConfiguration("run_uxrcedds_agent")),
            ),
        ]
    )
