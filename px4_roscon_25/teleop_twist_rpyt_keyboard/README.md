# teleop_twist_rpyt_keyboard

A ROS 2 (Humble) Python node for controlling PX4 drones using keyboard input with roll, pitch, yaw, and throttle (RPYT) commands via `geometry_msgs/Twist`.

This is ideal for sending manual velocity setpoints to PX4's `offboard` mode or a custom mode using ROS 2.

---

## 🚀 Features

- **Roll (A/D)** → `linear.y`
- **Pitch (W/S)** → `linear.x`
- **Yaw (Q/E)** → `angular.z`
- **Throttle (R/F)** → `linear.z`
- Adjustable speed with `T` (increase) and `Y` (decrease)
- Sends velocity commands to `/cmd_vel`
- Stops motion when no key is pressed

---

## Coordinate Frame

ROS uses the **ENU (East-North-Up)** body frame:

- `linear.x`: Forward
- `linear.y`: Left
- `linear.z`: Up
- `angular.z`: Yaw (CCW positive)

If you're converting to **PX4's NED (North-East-Down)** frame:

```cpp
velocity_ned.x = velocity_body.x * cos(yaw) - velocity_body.y * sin(yaw);
velocity_ned.y = velocity_body.x * sin(yaw) + velocity_body.y * cos(yaw);
velocity_ned.z = velocity_body.z;
```