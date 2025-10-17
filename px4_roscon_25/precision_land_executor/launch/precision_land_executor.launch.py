import os

from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node

def generate_launch_description():

    return LaunchDescription([
        Node(
            package="precision_land_executor",
            executable="precision_land_executor",
            name="precision_land_executor",
            output="screen",
            parameters=[
                {"use_sim_time": True}
            ]
        ),
        ExecuteProcess(
            cmd=[
                "gz", "service",
                "-s", "/world/walls/create",
                "--reqtype", "gz.msgs.EntityFactory",
                "--reptype", "gz.msgs.Boolean",
                "--timeout", "1000",
                "--req",
                (
                    'sdf_filename: "/home/ubuntu/PX4-gazebo-models/models/arucotag/model.sdf", '
                    'name: "arucotag", '
                    'pose: { position: { x: 8, y: -4.0, z: 0.001000 }, '
                    'orientation: { x: 0.0, y: 0.0, z: 0.0, w: 1.0 } }'
                )
            ],
            output="screen"
        )
    ])
