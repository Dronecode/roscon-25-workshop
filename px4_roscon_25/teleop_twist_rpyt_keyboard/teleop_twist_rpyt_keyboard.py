#!/usr/bin/env python
from platform import node
import rclpy
from rclpy.qos import QoSProfile, QoSReliabilityPolicy, QoSHistoryPolicy

from geometry_msgs.msg import Twist
from std_msgs.msg import Bool

import sys, select, termios, tty

settings = termios.tcgetattr(sys.stdin)

msg = """
Control PX4 drone with WASDQERF keys
------------------------------------
Movement:
  W/S : Pitch forward/backward
  A/D : Roll left/right
  Q/E : Yaw left/right
  R/F : Throttle up/down

Speed adjustment:
  T/G : Increase/decrease ALL speeds
  Y/H : Increase/decrease only linear
  U/J : Increase/decrease only angular

CTRL-C to quit
"""

# Format: (pitch, roll, throttle, yaw)
moveBindings = {
    'w': (1, 0, 0, 0),    # Pitch forward
    's': (-1, 0, 0, 0),   # Pitch backward
    'a': (0, -1, 0, 0),   # Roll left
    'd': (0, 1, 0, 0),    # Roll right
    'r': (0, 0, 1, 0),    # Throttle up
    'f': (0, 0, -1, 0),   # Throttle down
    'q': (0, 0, 0, 1),    # Yaw left
    'e': (0, 0, 0, -1),   # Yaw right
}

# Speed tuning bindings (linear, angular)
speedBindings = {
    't': (1.1, 1.1),  # Increase both
    'g': (0.9, 0.9),  # Decrease both
    'y': (1.1, 1.0),  # Increase linear only
    'h': (0.9, 1.0),  # Decrease linear only
    'u': (1.0, 1.1),  # Increase angular only
    'j': (1.0, 0.9),  # Decrease angular only
}


def getKey():
    tty.setraw(sys.stdin.fileno())
    select.select([sys.stdin], [], [], 0)
    key = sys.stdin.read(1)
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    return key


def vels(speed, turn):
    return "currently:\tspeed %.2f\tturn %.2f " % (speed, turn)


def main(args=None):
    if args is None:
        args = sys.argv

    rclpy.init()
    node = rclpy.create_node('teleop_twist_keyboard')

    qos = QoSProfile(
    reliability=QoSReliabilityPolicy.RELIABLE,
    history=QoSHistoryPolicy.KEEP_LAST,
    depth=10
    )
    pub = node.create_publisher(Twist, 'cmd_vel', qos)
    active_pub = node.create_publisher(Bool, '/teleop/active', qos)
    # Publish active=True at start
    active_msg = Bool()
    active_msg.data = True
    active_pub.publish(active_msg)

    speed = 0.5
    turn = 1.0
    pitch = roll = throttle = yaw = 0.0
    status = 0

    try:
        print(msg)
        print(vels(speed, turn))
        while True:
            key = getKey()
            if key in moveBindings:
                pitch, roll, throttle, yaw = moveBindings[key]
            elif key in speedBindings:
                speed *= speedBindings[key][0]
                turn *= speedBindings[key][1]
                print(vels(speed, turn))
                
                pitch = roll = throttle = yaw = 0.0

                if status == 14:
                    print(msg)
                status = (status + 1) % 15
            else:
                pitch = roll = throttle = yaw = 0.0

                if key == '\x03':
                    break

            twist = Twist()
            twist.linear.x = pitch * speed
            twist.linear.y = roll * speed
            twist.linear.z = -(throttle * speed)
            twist.angular.x = 0.0
            twist.angular.y = 0.0
            twist.angular.z = yaw * turn
            pub.publish(twist)
            # Publish active=True
            active_msg = Bool()
            active_msg.data = True
            active_pub.publish(active_msg)

    except Exception as e:
        print(e)

    finally:
        twist = Twist()
        pub.publish(twist)

        # Publish active=False
        active_msg.data = False
        active_pub.publish(active_msg)
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
        print("\nTeleop shutdown. Sent /teleop/active = False")

if __name__ == '__main__':
    main()
    