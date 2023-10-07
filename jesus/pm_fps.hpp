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

int32_t FPS_GetIdeal(pmove_t* pm);

zone_distance FPS_GetDistanceToZone(pmove_t* pm, int wishFPS);