# =====================================================================
# ORIGINAL VERSION
# =====================================================================
import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():

    pkg_share = FindPackageShare('offboard_demo').find('offboard_demo')

    clock_bridge_config_file = os.path.join(pkg_share,"cfg","clock_bridge.yaml")

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

    return LaunchDescription([
        run_uxrcedds_agent_arg,
        run_gz_clock_bridge_arg,
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
        Node(
            package="ros_gz_bridge",
            executable="parameter_bridge",
            name="gz_clock_bridge",
            parameters=[
                {"config_file": clock_bridge_config_file}
            ],
            condition=IfCondition(LaunchConfiguration("run_gz_clock_bridge"))
        ),
        ExecuteProcess(
            cmd=["MicroXRCEAgent", "udp4", "-p", "8888", "-v", "3"],
            output="screen",
            condition=IfCondition(LaunchConfiguration("run_uxrcedds_agent"))
        )
    ])

# # =====================================================================
# # ALTERNATIVE VERSION - For exercises
# # =====================================================================
# # import os

# from launch import LaunchDescription
# from launch.actions import DeclareLaunchArgument, ExecuteProcess
# from launch.conditions import IfCondition
# from launch.substitutions import LaunchConfiguration
# from launch_ros.actions import Node
# from launch_ros.substitutions import FindPackageShare

# def generate_launch_description():

#     pkg_share = FindPackageShare("offboard_demo").find("offboard_demo")

#     clock_bridge_config_file = os.path.join(pkg_share,"cfg","clock_bridge.yaml")

#     run_uxrcedds_agent_arg = DeclareLaunchArgument(
#         "run_uxrcedds_agent",
#         default_value="false",
#         description="Whether to run the MicroXRCEdds Agent",
#     )

#     run_gz_clock_bridge_arg = DeclareLaunchArgument(
#         "run_gz_clock_bridge",
#         default_value="false",
#         description="Whether to run the Gazebo clock bridge",
#     )

#     return LaunchDescription([
#         run_uxrcedds_agent_arg,
#         run_gz_clock_bridge_arg,
#         Node(
#             package="offboard_demo",
#             executable="offboard_demo",
#             name="offboard_demo",
#             output="screen",
#             parameters=[
#                 {"use_sim_time": True},
#                 {"takeoff_altitude": 3.0}
#             ]
#         ),
#         Node(
#             package="ros_gz_bridge",
#             executable="parameter_bridge",
#             name="gz_clock_bridge",
#             parameters=[
#                 {"config_file": clock_bridge_config_file}
#             ],
#             condition=IfCondition(LaunchConfiguration("run_gz_clock_bridge"))
#         ),
#         ExecuteProcess(
#             cmd=["MicroXRCEAgent", "udp4", "-p", "8888", "-v", "3"],
#             output="screen",
#             condition=IfCondition(LaunchConfiguration("run_uxrcedds_agent"))
#         )
#     ])