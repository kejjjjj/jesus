#pragma once

#include "pch.hpp"

void lineup_testing_func(usercmd_s* cmd);

class Lineup
{
public:
	Lineup(const fvec3& to, const fvec3& _destination_angles, float _lineup_dist) :
		dst(to),
		destination_angles(_destination_angles),
		lineup_dist(_lineup_dist)
	{
		prepare_for_journey();
	}

	void move(usercmd_s* cmd) noexcept;
	bool has_finished() const noexcept {return finished; }
	bool needs_restart() const noexcept { return restart_required; }
	bool gave_up() const noexcept { return give_up; }
private:

	void update_viewangles(usercmd_s* cmd) noexcept;
	void update_origin() noexcept;  
	void move_closer(usercmd_s* cmd) noexcept;

	bool can_pathfind_to_target() noexcept;

	bool waiting_for_prediction_accuracy() const noexcept { return time_until_prediction_check > 0; }
	bool test_prediction_accuracy() noexcept { return --time_until_prediction_check == 0; }
	fvec3 predict_stop_position();

	void prepare_for_journey() noexcept;

	fvec3 dst;
	fvec3 destination_angles;
	fvec3 delta_angles;

	float lineup_dist = 0.01f;
	float total_distance = 0.f;
	float distance_moved = 0.f;
	const float(&viewangles)[3] = {clients->cgameViewangles};
	const float(&origin)[3] = {clients->cgameOrigin};
	fvec3 old_origin;
	fvec3 predicted_pos;

	bool rng_crouch = false;

	float yaw2target = 0.f;

	unsigned short time_until_prediction_check = 0;
	bool viewangles_ok = false;
	bool finished = false;
	bool restart_required = false;
	bool give_up = false;
	Lineup(const Lineup&) = delete;
	Lineup& operator=(const Lineup&) = delete;
};