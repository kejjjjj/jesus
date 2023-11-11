#pragma once

#include "pch.hpp"



struct elebot_detection_data
{
	std::list<playback_cmd> playback;
	bool is_valid = false;
	fvec3 normals;
	Axis_t hitAxis = Axis_t::X;
	float destination = 0.f;
	float lineup_trigger = 0.f;
	float target_yaw = 0.f;
	sc_winding_t face;
	bool saved = false;
	cbrush_t* brush = 0;
	fvec3 brush_origin;

	bool has_winding() const noexcept { return face.points.empty() == false; }
};

struct elebot_export_data
{	

	std::list<playback_cmd> playback;
	fvec3 normals;
	Axis_t hitAxis = Axis_t::X;
	float destination = 0.f;
	float lineup_trigger = 0.f;
	float target_yaw = 0.f;
	fvec3 brush_origin;
};
void elebot_on_disconnect();
void elebot_save_selected();
void elebot_load_from_map(const char* mapname);

bool elebot_player_is_next_to_ele_surface(playerState_s* ps);

void elebot_evaluate_angles_midair(playerState_s* ps);
void elebot_render_winding(GfxViewParms* viewParms);
bool elebot_has_lineup(playerState_s* ps, usercmd_s* cmd);
void elebot_start_lineup(playerState_s* ps, usercmd_s* cmd);
void elebot_start_playback(playerState_s* ps, usercmd_s* cmd);