#pragma once
#include "pch.hpp"

enum fps_enum
{
	F333,
	F250,
	F200,
	F125
};
struct fps_zone
{
	float start;
	float end;
	int FPS;
	float length;
};

struct zone_distance
{
	float begin;
	float end;
	float length;
};

std::vector<fps_zone> FPS_GetZones(int g_speed);

int32_t FPS_GetIdeal(playerState_s* ps, usercmd_s* cmd);


zone_distance FPS_GetDistanceToZone(playerState_s* ps, usercmd_s* cmd, int wishFPS);

namespace fps
{
	inline bool refresh_required = false;
	inline bool distances_refresh_required = false;
}