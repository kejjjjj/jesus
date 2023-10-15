#pragma once

#include "pch.hpp"

signed int BG_BulletDamage(WeaponDef* weapon, BulletFireParams* fireParams, BulletTraceResults* trace);
float BG_GetSurfacePenetrationDepth(int surfaceType, WeaponDef* weapon);
void G_CalculateBulletPenetration(BulletFireParams* bfp, WeaponDef* weapon, vec3_t start, vec3_t end, int ignoreId, int clientNum, vec3_t forward, vec3_t right, vec3_t up);
void CG_BulletEndpos(float* upDir, float* end, int randSeed, float spread, float* start, float* dir, float* forwardDir, float* rightDir, float maxRange);