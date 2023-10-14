#pragma once

#include "pch.hpp"

constexpr float destination_reached() { return 1.f; }
constexpr float destination_overstep() { return -1.f; }

class Elebot
{
public:
	static Elebot& getInstance() {
		static Elebot e;
		return e;
	}

	void start_ground(const float destination, const cardinalDirection_e direction, const Axis_t axis);
	static void start_ground();

	void move(usercmd_s* cmd);
	void do_playback(usercmd_s* cmd);

private:
	void move_closer(usercmd_s* cmd);
	bool move_forward(usercmd_s* cmd);
	bool move_sideways(usercmd_s* cmd);
	float eval_angle(usercmd_s* cmd) noexcept;
	bool is_interrupted() const noexcept;
	bool move_wait_for_prediction() noexcept {
		if (test_prediction)
		{
			test_prediction--;
			return true;
		}
		return false;
	}
	bool is_step_ele(const fvec3& fwd_angles) const noexcept;
	void reset_prediction() noexcept;
	void update_origin() noexcept;

	float P_PredictNextPosition(usercmd_s* cmd, char forwardmove, char rightmove, float yaw);
	bool prediction_failed() const noexcept { return origin != predicted_origin; };
	void init(const float start, const float destination, const Axis_t axis, const cardinalDirection_e direction);

	bool initialized = false;
	bool step_ele = false;

	Axis_t destinationAxis = Axis_t::X;
	float destination = 0.f;
	float start = 0.f;
	float distance_moved = 0.f;
	float origin = 0.f;
	float old_origin = 0.f;
	float positionZ = 0.f;
	float predicted_origin = 0.f;

	std::optional<float> angle_delta;
	
	int& fps = Dvar_FindMalleableVar("com_maxfps")->current.integer;

	int test_prediction = 8;
	bool bmove_forward = true;
	float total_distance = 0.f;
	int frameTime = 5;
	pmove_t pm;
	pml_t pml;
	playerState_s ps;

	//pmove_t predicted_pm;
	//pml_t predicted_pml;
	//playerState_s predicted_ps;

	Playback playback;
	std::list<playback_cmd> cmds;
};