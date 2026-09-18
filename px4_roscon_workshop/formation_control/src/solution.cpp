#include <formation_control.hpp>
#include <formation_executor_solution.hpp>

#include "rclcpp/rclcpp.hpp"

static const std::string kNodeName = "formation_controller";
static const bool kEnableDebugOutput = true;

FormationExecutor::FormationExecutor(px4_ros2::ModeBase& formation_mode)
    : ModeExecutorBase(Settings{}, formation_mode)
{
}

void FormationExecutor::onActivate()
{
  switchToState(State::Takeoff, px4_ros2::Result::Success);
}

void FormationExecutor::onDeactivate(DeactivateReason reason)
{
  RCLCPP_INFO(node().get_logger(), "Formation executor deactivated: %d",
              static_cast<int>(reason));
}

void FormationExecutor::switchToState(State state, px4_ros2::Result previous_result)
{
  if (previous_result != px4_ros2::Result::Success) {
    RCLCPP_WARN(node().get_logger(), "Formation sequence stopped in state %d with result %d",
                static_cast<int>(state), static_cast<int>(previous_result));
    return;
  }

  switch (state) {
    case State::Takeoff:
      takeoff(
          [this](px4_ros2::Result result) {
            switchToState(State::Formation, result);
          },
          2.0f);
      break;
    case State::Formation:
      scheduleMode(ownedMode().id(),
                   [this](px4_ros2::Result result) {
                     switchToState(State::WaitUntilDisarmed, result);
                   });
      break;
    case State::WaitUntilDisarmed:
      waitUntilDisarmed([](px4_ros2::Result) {});
      break;
  }
}

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

  FormationControlMode mode(*node, node->get_parameter("px4_ns").as_string());
  FormationExecutor executor(mode);
  if (!executor.doRegister()) {
    RCLCPP_ERROR(node->get_logger(), "Formation executor registration failed");
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}