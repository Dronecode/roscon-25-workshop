#include <cstdio>

#include "px4_tf/px4_tf_publisher_node.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Px4TfPublisherNode>());
  rclcpp::shutdown();
  return 0;
}
