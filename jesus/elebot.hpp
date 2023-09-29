#pragma once

#include "pch.hpp"


class Elebot
{
public:
	static Elebot& getInstance() {
		static Elebot e;
		return e;
	}

	static void start_ground();

	void move(usercmd_s* cmd);
	void do_playback(usercmd_s* cmd);

private:
	float start_simulation();
	bool step_is_too_big(const float new_position);
	void update_stats(const float new_position);
	float get_distance_to_end() const noexcept { return distanceMoved <= 0.125 ? 0.125f - distanceMoved : distanceMoved; }
	bool initialized = false;

	Axis_t destinationAxis = Axis_t::X;
	float destination = 0.f;
	float start = 0.f;
	float distanceMoved = 0.f;
	float position = 0.f;
	float predicted_position = 0.f;

	bool moveForward = true;

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