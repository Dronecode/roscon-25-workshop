#pragma once

#include <px4_ros2/components/mode_executor.hpp>
#include <px4_ros2/components/node_with_mode.hpp>

#include "VtolCustomMode.hpp"

class VtolCustomModeExecutor : public px4_ros2::ModeExecutorBase {
public:
    VtolCustomModeExecutor(px4_ros2::ModeBase &owned_mode);

    // See ModeExecutorBase
    void onActivate() override;
    void onDeactivate(DeactivateReason reason) override;

private:
    // State management. VtolCustomMode owns both VTOL transitions
    // internally, so the executor only has to bracket it with the inbuilt
    // Takeoff/RTL modes.
    enum class State {
        Takeoff,          // Inbuilt takeoff mode, multicopter form
        Cruise,           // VtolCustomMode: transition, fixed-wing cruise, transition back
        ReturnToLaunch,   // Inbuilt RTL mode: fly back to home and land there, multicopter form
        WaitUntilDisarmed // Final state, wait until the vehicle is disarmed
    };
    State _state;
    void switchToState(State state, px4_ros2::Result previous_result);

    float _param_takeoff_altitude_amsl = {};
};
