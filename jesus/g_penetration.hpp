#pragma once

#include "pch.hpp"

signed int BG_BulletDamage(WeaponDef* weapon, BulletFireParams* fireParams, BulletTraceResults* trace);
float BG_GetSurfacePenetrationDepth(int surfaceType, WeaponDef* weapon);
void G_CalculateBulletPenetration(BulletFireParams* bfp, WeaponDef* weapon, BulletTraceResults* br, vec3_t start, vec3_t end, int ignoreId, int clientNum);
void CG_BulletEndpos(float* upDir, float* end, int randSeed, float spread, float* start, float* dir, float* forwardDir, float* rightDir, float maxRange);
bool BulletTrace(BulletTraceResults* trace, BulletFireParams* fireParams, centity_s* ignore, int depthSurfaceType);
bool BG_AdvanceTrace(BulletTraceResults* br, BulletFireParams* bpp, float fraction);

void CG_BulletEndPosition(float* upDir, float* end, int randSeed, float spread, float* start, float* dir, float* forwardDir, float* rightDir, float maxRange);


BulletTraceResults FireBulletPenetrate(centity_s* ignoreEnt, int clientNum, BulletFireParams* fireParams, WeaponDef* _weapon, float* tracerStart, bool spawnTracer);

struct spreadData
{
	static spreadData& get() { static spreadData d; return d; }

	fvec3 bullet_endpos;
	fvec3 angles;
	bool weapon_fired = false;
	fvec3 dir;
	fvec3 resolved_angles;
};

inline fvec3 bullet_endpos;