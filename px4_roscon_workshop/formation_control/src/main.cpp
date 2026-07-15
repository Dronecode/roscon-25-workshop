#include <formation_control.hpp>

#include "rclcpp/rclcpp.hpp"

static const std::string kNodeName = "formation_controller";
static const bool kEnableDebugOutput = true;

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>(kNodeName);
  node->declare_parameter("px4_ns", "");
  node->declare_parameter("tf_prefix", "");
  node->declare_parameter("neighbor_distances", std::vector<double>{});
  node->declare_parameter("neighbor_prefixes", std::vector<std::string>{});
  node->declare_parameter("gain", 1.0);
  
  if (kEnableDebugOutput) {
    auto ret = rcutils_logging_set_logger_level(node->get_logger().get_name(),
                                                RCUTILS_LOG_SEVERITY_DEBUG);

    if (ret != RCUTILS_RET_OK) {
      RCLCPP_ERROR(node->get_logger(), "Error setting severity: %s",
                   rcutils_get_error_string().str);
      rcutils_reset_error();
    }
  }

  FlightModeTest mode(*node, node->get_parameter("px4_ns").as_string());
  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}