/****************************************************************************
 * Copyright (c) 2026 PX4 Development Team.
 * SPDX-License-Identifier: BSD-3-Clause
 ****************************************************************************/

#include <mode.hpp>
#include <px4_ros2/components/node_with_mode.hpp>

#include "rclcpp/rclcpp.hpp"

using MyNodeWithMode = px4_ros2::NodeWithMode<MotorRampMode>;

static const std::string kNodeName = "motor_ramp_demo";
static const bool kEnableDebugOutput = true;

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MyNodeWithMode>(kNodeName, kEnableDebugOutput));
  rclcpp::shutdown();
  return 0;
}
