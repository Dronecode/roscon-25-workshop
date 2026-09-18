#include "MousePointerGimbalControl.hpp"
#include "rclcpp/rclcpp.hpp"

void MousePointerGimbalControlNode::controlLoop()
{
  /*
    Exercise: convert the latest mouse pointer position into a gimbal pan/tilt rate and publish
    it as a geometry_msgs::msg::Twist on _cmd_vel_pub (linear.x = pan rate, linear.y = tilt
    rate, both in deg/s).

    Available state:
    - _last_point: latest pointer position (nullptr until the first message arrives).
    - _image_width / _image_height: latest camera image size in pixels (0 until a
      sensor_msgs::msg::CameraInfo message arrives).

    Steps:
    - Normalize the pointer position to the [-1, 1] range around the image center
      (+x right, +y down).
    - Scale the normalized x/y by _param_max_pan_rate / _param_max_tilt_rate (deg/s).
    - Zero out small deflections within _param_deadzone of the center.
    - Zero the output if the pointer is stale (see _last_point_time and
      _param_pointer_timeout) or no image size has been received yet.
    - _param_invert_tilt: image y grows downward, so tilt is inverted by default.
  */

  geometry_msgs::msg::Twist msg{};
  _cmd_vel_pub->publish(msg);
}

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MousePointerGimbalControlNode>());
  rclcpp::shutdown();
  return 0;
}
