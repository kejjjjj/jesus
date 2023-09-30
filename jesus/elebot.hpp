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

	bool is_step_ele(const fvec3& fwd_angles) const noexcept;

	void init(const float start, const float destination, const Axis_t axis, const cardinalDirection_e direction);
	float start_simulation();
	bool step_is_too_big(const float new_position);
	void forward_on_exceeded(const float new_position) noexcept;
	void angled_on_exceeded(const float new_position) noexcept;
	float get_fitness_for_move(const float new_position) noexcept;
	float get_overstep_fitness(const float new_position) noexcept;
	void update_deltas(usercmd_s* cmd, const float new_position);
	void on_destination_reached(usercmd_s* cmd);
	void on_impossible_coordinate(usercmd_s* cmd, const float new_position);
	void reset_prediction() noexcept;


	float get_new_delta() const noexcept;
	void update_stats(const float new_position);
	float get_distance_to_end() const noexcept { return distanceMoved <= 0.125 ? 0.125f - distanceMoved : distanceMoved; }
	bool initialized = false;
	bool step_ele = false;

	Axis_t destinationAxis = Axis_t::X;
	float destination = 0.f;
	float start = 0.f;
	float distanceMoved = 0.f;
	float position = 0.f;
	float positionZ = 0.f;
	float predicted_position = 0.f;
	float base_angle = 0.f;

	bool moveForward = true;
	float current_delta = 90.f;
	float lowest_delta = 0.f;
	float best_delta = 0.f;
	float best_fitness = 0.f;
	float delta_increment = 0.01f;
	float backup_delta = 0.f;
	float total_distance = 0.f;
	pmove_t pm;
	pml_t pml;
	playerState_s ps;

	//pmove_t predicted_pm;
	//pml_t predicted_pml;
	//playerState_s predicted_ps;

	Playback playback;
	std::list<playback_cmd> cmds;
	int testPlayback = 0;


	int frameTime = (1000 / 200); //start with 200fps and then start increasing from there

};