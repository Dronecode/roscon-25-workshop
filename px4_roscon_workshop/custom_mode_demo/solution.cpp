#include <px4_ros2/components/mode.hpp>
#include <px4_ros2/components/node_with_mode.hpp>
#include <px4_ros2/control/setpoint_types/multicopter/goto.hpp>
#include <px4_ros2/odometry/local_position.hpp>

#include <rclcpp/rclcpp.hpp>

#include <Eigen/Eigen>
#include <memory>
#include <vector>

static const std::string kModeNameCustomWaypoints = "CustomWaypoints";
static const std::string kNodeName = "custom_mode_demo";
static const bool kEnableDebugOutput = true;

// Solution: same waypoint mission as CustomMode.cpp, but driven by go-to
// setpoints (smooth position/heading control) instead of trajectory setpoints.
class CustomWaypointsGoto : public px4_ros2::ModeBase {
public:
    explicit CustomWaypointsGoto(rclcpp::Node &node)
        : px4_ros2::ModeBase(node, kModeNameCustomWaypoints),
          _node(node)
    {
        _goto_setpoint = std::make_shared<px4_ros2::MulticopterGotoSetpointType>(*this);
        _local_position = std::make_shared<px4_ros2::OdometryLocalPosition>(*this);

        RCLCPP_INFO(node.get_logger(), "CustomWaypointsGoto mode initialized.");
    }

    void onActivate() override
    {
        _trajectory_waypoints.clear();
        _trajectory_waypoints.push_back(Eigen::Vector3f(5.0f, 0.0f, -1.5f));
        _trajectory_waypoints.push_back(Eigen::Vector3f(5.0f, 5.0f, -1.5f));
        _trajectory_waypoints.push_back(Eigen::Vector3f(-5.0f, 5.0f, -1.5f));
        _trajectory_waypoints.push_back(Eigen::Vector3f(-5.0f, -5.0f, -1.5f));
        _trajectory_waypoints.push_back(Eigen::Vector3f(5.0f, -5.0f, -1.5f));
        _trajectory_waypoints.push_back(Eigen::Vector3f(5.0f, 0.0f, -1.5f));
        _trajectory_waypoints.push_back(Eigen::Vector3f(0.0f, 0.0f, -1.5f));

        _current_waypoint_index = 0;
        RCLCPP_INFO(_node.get_logger(), "CustomWaypointsGoto mode activated");
    }

    void onDeactivate() override
    {
        RCLCPP_INFO(_node.get_logger(), "CustomWaypointsGoto mode deactivated");
    }

    void updateSetpoint([[maybe_unused]] float dt_s) override
    {
        if (_current_waypoint_index < _trajectory_waypoints.size()) {
            const auto &current_waypoint = _trajectory_waypoints[_current_waypoint_index];
            _goto_setpoint->update(current_waypoint);

            if (positionReached(current_waypoint)) {
                _current_waypoint_index++;
            }
        } else {
            RCLCPP_INFO_ONCE(_node.get_logger(), "All waypoints completed.");
            completed(px4_ros2::Result::Success);
            return;
        }
    }

private:
    bool positionReached(const Eigen::Vector3f &target_position_m) const
    {
        static constexpr float kPositionErrorThreshold = 0.5f;  // [m]
        const Eigen::Vector3f position_error_m =
            target_position_m - _local_position->positionNed();
        return position_error_m.norm() < kPositionErrorThreshold;
    }

    rclcpp::Node &_node;

    std::shared_ptr<px4_ros2::MulticopterGotoSetpointType> _goto_setpoint;
    std::shared_ptr<px4_ros2::OdometryLocalPosition> _local_position;

    std::vector<Eigen::Vector3f> _trajectory_waypoints;
    size_t _current_waypoint_index{};
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<px4_ros2::NodeWithMode<CustomWaypointsGoto>>(
        kNodeName, kEnableDebugOutput));
    rclcpp::shutdown();
    return 0;
}
