#include "pch.hpp"

float CG_CalcPlayerHealth(playerState_s* ps)
{
	float health = 0.f;
	__asm
	{
		mov eax, ps;
		mov esi, 0x442640;
		call esi;
		fstp health;
	}

	return health;
}
float CG_GetDistanceToGround(playerState_s* ps)
{
	trace_t trace;

	fvec3 down_angles = { 90, 0, 0 };

	fvec3 end = down_angles.toforward() * 99999.f;

	CG_TracePoint(vec3_t{ 14,14, 12 }, &trace, ps->origin, vec3_t{ -14,-14,0 }, end, ps->clientNum, MASK_PLAYERSOLID, 0, 0);

	return abs((ps->origin[2] + trace.fraction * (end[2] - ps->origin[2])) - ps->origin[2]);
}
