#pragma once
#include "pch.hpp"

struct range_t
{
	float x1;
	float x2;
	bool split;
};
enum class Axis_t : int
{
	X,
	Y,
	Z //?
};

enum class cardinalDirection_e : short
{
	N = 0, //X+ 0°
	E = -90, //Y- 270°
	S = 180, //X- 180°
	W = 90 //Y+ 90°

};


void CG_SetYaw(const float ang);
void CG_SetPitch(const float ang);
void CG_SetRoll(const float ang);
void CG_SetPlayerAngles(const fvec3& target);

void CL_SetSilentAngles(const fvec3& target);

void CL_SetPlayerAngles(usercmd_s* cmd, float* delta_angles, const fvec3& target);
ivec3 CL_GetPlayerAngles(usercmd_s* cmd, float* delta_angles, const fvec3& target);

bool CG_SetDeltaAngles(float* dst);

Axis_t CG_GetAxisBasedOnYaw(float yaw);
float CG_GetNearestCardinalAngle();
float CG_GetNearestCardinalAngle(float yaw);
cardinalDirection_e CG_RoundAngleToCardinalDirection(const float a);

void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
float ProjectionX(float angle, float fov);
range_t AnglesToRange(float start, float end, float yaw, float fov);

float AngleNormalizePI(float angle);
float AngleNormalize360(float angle);
float AngleNormalize180(float angle);
float AngleNormalize90(float angle);

float AngleDelta(float angle1, float angle2);

float AngularDistance(float value1, float value2);

bool PointWithinLine(const fvec3& start, const fvec3& end, const fvec3& point, float radius);

std::optional<float> CG_GetOptYawDelta(pmove_t* pm, pml_t* pml);
std::optional<float> CG_GetOptYawDelta(playerState_s* ps, usercmd_s* cmd, usercmd_s* oldcmd);