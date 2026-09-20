#!/usr/bin/env python3
"""Fake rover pose publisher for testing sar_modes without a rover model.

Publishes a moving PoseStamped on /rover/pose (ROS ENU): stands still for
start_delay seconds once at startup, then loops straight-line driving at
min_linear_speed (straight_duration seconds) and circling at max_linear_speed
(circle_radius, num_circle_turns turns) back to back until the node is
stopped, the speed change between phases lets a speed-based mode switcher
(e.g. sar_auto_executor) actually exercise both sides of its threshold in a
single run, rather than needing a relaunch with a different speed.
"""
import math

import rclpy
from geometry_msgs.msg import PoseStamped
from rclpy.node import Node


class FakeRoverMover(Node):

    def __init__(self):
        super().__init__('fake_rover_mover')

        self.declare_parameter('start_x', 5.0)
        self.declare_parameter('start_y', 3.0)
        self.declare_parameter('start_yaw_deg', 45.0)
        self.declare_parameter('min_linear_speed', 0.2)    # m/s, used during the straight phase
        self.declare_parameter('max_linear_speed', 0.8)    # m/s, used during the circle phase
        self.declare_parameter('start_delay', 3.0)         # seconds standing still before motion begins
        self.declare_parameter('straight_duration', 45.0)  # seconds driving straight (30-60s is a good range, hopefully)
        self.declare_parameter('circle_radius', 5.0)       # meters, clamped to a 5m minimum
        self.declare_parameter('num_circle_turns', 2.0)    # full 360 deg turns before holding
        self.declare_parameter('rate_hz', 10.0)

        self._x = self.get_parameter('start_x').value
        self._y = self.get_parameter('start_y').value
        self._yaw = math.radians(self.get_parameter('start_yaw_deg').value)
        self._min_speed = self.get_parameter('min_linear_speed').value
        self._max_speed = self.get_parameter('max_linear_speed').value
        self._start_delay = self.get_parameter('start_delay').value
        self._straight_duration = self.get_parameter('straight_duration').value
        self._circle_radius = max(self.get_parameter('circle_radius').value, 5.0)
        self._num_circle_turns = self.get_parameter('num_circle_turns').value
        rate_hz = self.get_parameter('rate_hz').value

        self._dt = 1.0 / rate_hz

        # Angular rate that makes the circle phase trace circle_radius at max_linear_speed.
        self._circle_omega = self._max_speed / self._circle_radius if self._max_speed > 0.0 else 0.0
        self._circle_duration = (
            self._num_circle_turns * (2.0 * math.pi / self._circle_omega)
            if self._circle_omega > 0.0 else 0.0
        )

        # 'hold' happens once at startup; 'straight' <-> 'circle' then loop forever.
        self._phase = 'hold'
        self._phase_elapsed = 0.0

        self._pub = self.create_publisher(PoseStamped, '/rover/pose', 10)
        self._timer = self.create_timer(self._dt, self._on_timer)

        self.get_logger().info(
            f'Fake rover starting at ({self._x:.1f}, {self._y:.1f}), yaw {math.degrees(self._yaw):.0f} deg. '
            f'Holds {self._start_delay:.1f}s, then loops: straight at {self._min_speed} m/s for '
            f'{self._straight_duration:.1f}s, circle at {self._max_speed} m/s (r={self._circle_radius:.1f}m) '
            f'for {self._num_circle_turns:.1f} turns, repeat.'
        )

    def _on_timer(self):
        self._phase_elapsed += self._dt

        if self._phase == 'hold':
            if self._phase_elapsed >= self._start_delay:
                self._phase, self._phase_elapsed = 'straight', 0.0
        elif self._phase == 'straight':
            self._x += self._min_speed * math.cos(self._yaw) * self._dt
            self._y += self._min_speed * math.sin(self._yaw) * self._dt
            if self._phase_elapsed >= self._straight_duration:
                self._phase, self._phase_elapsed = 'circle', 0.0
        elif self._phase == 'circle':
            self._yaw += self._circle_omega * self._dt
            self._x += self._max_speed * math.cos(self._yaw) * self._dt
            self._y += self._max_speed * math.sin(self._yaw) * self._dt
            if self._phase_elapsed >= self._circle_duration:
                self._phase, self._phase_elapsed = 'straight', 0.0

        msg = PoseStamped()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = 'map'
        msg.pose.position.x = self._x
        msg.pose.position.y = self._y
        msg.pose.position.z = 0.0
        msg.pose.orientation.z = math.sin(self._yaw / 2.0)
        msg.pose.orientation.w = math.cos(self._yaw / 2.0)
        self._pub.publish(msg)


def main():
    rclpy.init()
    node = FakeRoverMover()
    try:
        rclpy.spin(node)
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
