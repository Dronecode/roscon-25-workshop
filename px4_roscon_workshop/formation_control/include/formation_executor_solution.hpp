#pragma once

#include <px4_ros2/components/mode_executor.hpp>

class FormationExecutor : public px4_ros2::ModeExecutorBase {
 public:
	explicit FormationExecutor(px4_ros2::ModeBase& formation_mode);

	void onActivate() override;
	void onDeactivate(DeactivateReason reason) override;

 private:
	enum class State {
		Takeoff,
		Formation,
		WaitUntilDisarmed,
	};

	void switchToState(State state, px4_ros2::Result previous_result);
};
