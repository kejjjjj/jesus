#include "pch.hpp"

signed int BG_BulletDamage(WeaponDef* weapon, BulletFireParams* fireParams, BulletTraceResults* trace)
{
	int minDamage;
	int damage;
	double distance;
	float difference;
	int fireParamsa;
	float bulletEffectb;
	float bulletEffectc;

	minDamage = weapon->minDamage;
	difference = weapon->fMinDamageRange - weapon->fMaxDamageRange;
	damage = weapon->damage;
	fireParamsa = damage;
	if (damage != minDamage && 0.0 != difference)
	{
		distance = Distance(trace->hitPos, fireParams->origStart);
		if (weapon->fMaxDamageRange > distance)
		{
			return (fireParams->damageMultiplier * damage);
		}
		if (weapon->fMinDamageRange > distance)
		{
			bulletEffectb = (distance - weapon->fMaxDamageRange) / difference;
			bulletEffectc = bulletEffectb * minDamage + (1.0 - bulletEffectb) * damage;
			return (fireParams->damageMultiplier * bulletEffectc);
		}
		fireParamsa = minDamage;
	}
	return (fireParams->damageMultiplier * fireParamsa);

}
float BG_GetSurfacePenetrationDepth(int surfaceType, WeaponDef* weapon)
{


	float result;
	
	__asm
	{
		mov ecx, weapon;
		mov eax, surfaceType;
		mov esi, 0x415FE0;
		call esi;
		fstp result;
	}
	return result;

	//if (surfaceType)
	//{
	//	result = penetrationDepthTable[29 * weapon->penetrateType + surfaceType];
	//}
	//else
	//{
	//	result = 0.0;
	//}
	//return result;
}
void G_CalculateBulletPenetration(BulletFireParams* bfp, WeaponDef* weapon, BulletTraceResults* br, vec3_t start, vec3_t end, int ignoreId, int clientNum)
{


	centity_s* ignoreEnt = &centity[ignoreId];

	vec3_t dir;

	VectorCopy(start, bfp->start);
	VectorCopy(end, bfp->end);
	VectorCopy(start, bfp->origStart);
	bfp->methodOfDeath = 1;
	bfp->ignoreEntIndex = ignoreId;
	bfp->damageMultiplier = 1;
	bfp->weaponEntIndex = ignoreId;

	clientNum = 0;

	trace_t trace;

	CG_TracePoint(vec3_origin->extents.start, &trace, bfp->start, vec3_origin->extents.start, bfp->end, ignoreId, 0x2806831, 0, 0);

	if (trace.fraction == 1.f) {
		VectorCopy(end, br->hitPos);
		return;
	}

	dir[0] = end[0] - start[0];
	dir[1] = end[1] - start[1];
	dir[2] = end[2] - start[2];

	float damageRange = 0;
	int shotCount = 0;

	bool spreadWeapon = weapon->weapClass == WEAPCLASS_SPREAD;
	
	if (spreadWeapon) {
		damageRange = weapon->fMinDamageRange;
		shotCount = weapon->shotCount;
	}
	else {
		damageRange = 8192.0f;
		shotCount = 1;
	}

	fvec3 angles = fvec3(dir).toangles();

	vec3_t forward;
	vec3_t right;
	vec3_t up;



	AngleVectors(angles, forward, right, up);

	
	VectorNormalize(dir);
	VectorCopy(dir, bfp->dir);

	
	fvec3 tracerStart = rg->viewOrg;

	////std::cout << "tracerStart: " << tracerStart << '\n';

	float len = std::min(sqrtf(tracerStart.mag()), 255.f);

	float maxSpread = weapon->hipSpreadStandMax;
	float spread = len / 255.f;

	spread = (maxSpread - weapon->fAdsSpread) * spread + weapon->fAdsSpread;

	for (int shot = 0; shot < shotCount; shot++) {

		//VectorCopy(start, _start);
		VectorCopy(start, bfp->start);
		VectorCopy(forward, bfp->dir);



		CG_BulletEndpos(up, bfp->end, shot + cgs->predictedPlayerState.commandTime,
			0.f, bfp->start, bfp->dir, forward, right, damageRange);


		bfp->damageMultiplier = 1.f;

		*br = FireBulletPenetrate(ignoreEnt, clientNum, bfp, weapon, start, false);
	}
	if (bfp->damageMultiplier < 0)
		bfp->damageMultiplier = 0;

	return;

}
void CG_BulletEndpos(float* upDir, float* end, int randSeed, float spread, float* start, float* dir, float* forwardDir, float* rightDir, float maxRange)
{
	double v10; // st7
	double v11; // st7
	float x; // [esp+4h] [ebp-Ch] BYREF
	float y; // [esp+8h] [ebp-8h] BYREF
	float aimOffset; // [esp+Ch] [ebp-4h]

	y = spread * 0.01745329238474369;
	y = tan(y);
	aimOffset = y * maxRange;

	__asm {
		lea eax, y;
		push eax;
		lea ecx, x;
		push ecx;
		mov edx, randSeed;
		push edx;
		mov esi, 0x4A66E0;
		call esi;
		add esp, 0xC;
	}
	x = x * aimOffset;
	y = aimOffset * y;
	*end = *forwardDir * maxRange + *start;
	end[1] = forwardDir[1] * maxRange + start[1];
	end[2] = maxRange * forwardDir[2] + start[2];
	v10 = x;
	*end = *rightDir * x + *end;
	end[1] = rightDir[1] * v10 + end[1];
	end[2] = v10 * rightDir[2] + end[2];
	v11 = y;
	*end = *upDir * y + *end;
	end[1] = upDir[1] * v11 + end[1];
	end[2] = v11 * upDir[2] + end[2];
	if (dir)
	{
		*dir = *end - *start;
		dir[1] = end[1] - start[1];
		dir[2] = end[2] - start[2];
		VectorNormalize(dir);
	}


}
bool BulletTrace(BulletTraceResults* trace, BulletFireParams* fireParams, centity_s* ignore, int depthSurfaceType)
{
	bool result = false;
	__asm
	{
		push depthSurfaceType;
		push ignore;
		push fireParams;
		mov eax, trace;
		mov esi, 0x4558E0;
		call esi;
		add esp, 0xC;
		mov result, al;
	}

	return result;
}
bool BG_AdvanceTrace(BulletTraceResults* br, BulletFireParams* bpp, float fraction)
{
	bool result = false;

	__asm
	{
		push fraction;
		mov ecx, bpp;
		mov edx, br;
		mov esi, 0x415EE0;
		call esi;
		add esp, 0x4;
		mov result, al;
	}
	return result;
}
BulletTraceResults FireBulletPenetrate(centity_s* ignoreEnt, int clientNum, BulletFireParams* fireParams, WeaponDef* weapon, float* tracerStart, bool spawnTracer)
{
	int weaponIndex = 0;
	BulletTraceResults trace;
	int hitId = 0;
	float penetrationDepth = 0.f;
	fvec3 hitpos;
	BulletFireParams bpp;
	BulletTraceResults br;
	float* fireEnd;
	float* fireStart;

	while (weapon != BG_WeaponNames[weaponIndex]) {
		if (++weaponIndex > *bg_lastParsedWeaponIndex) {
			weaponIndex = 0;
			break;
		}
	}

	if (weapon->weapType) {
		spawnTracer = false;
	}

	if (!BulletTrace(&trace, fireParams, ignoreEnt, 0)){
		//if (spawnTracer)
		//{
		//	CG_SpawnTracer(fireParams->end, tracerStart, clientNum);
		//}
		return trace;
	}
	
	if (trace.trace.hitType == TRACE_HITTYPE_DYNENT_MODEL || trace.trace.hitType == TRACE_HITTYPE_DYNENT_BRUSH)
		hitId = 1022;
	else if (trace.trace.hitType == TRACE_HITTYPE_ENTITY)
		hitId = trace.trace.hitId;
	else
		hitId = 1023;
	
	//std::cout << fvec3(trace.hitPos) << '\n';

	//if (spawnTracer)
	//{
	//	CG_SpawnTracer(fireParams->end, tracerStart, clientNum);
	//}

	/*if ( weapon->weapType == WEAPTYPE_BULLET )
	  {
		DynEntCl_EntityImpactEvent(&trace, clientNum, ignoreEnt->nextState.number, fireParams->start, trace.hitPos, 0);
		DynEntCl_DynEntImpactEvent(clientNum, ignoreEnt->nextState.number, fireParams->start, trace.hitPos, _weapon->damage, 0);
		CG_BulletHitEvent(hitId, ignoreEnt->nextState.number, clientNum, _weaponIndex, fireParams->start, trace.hitPos, trace.trace.normal, (trace.trace.surfaceFlags >> 20) & 0x1F, 0, _weapon->damage, trace.trace.contents);
	  }*/

	if (weapon->penetrateType && !trace.trace.startsolid)
	{
		int numHits = 0;
		bool traceHit = false;
		float inverseZ = 0.f;
		bool old_solid = false;
		bool new_solid= false;
		float normalZ = 0.f;
		float surfacePenetrationDepth = 0.f;
		int surfFlags = 0;
		dvar_s* bullet_penetrationMinFxDist = Dvar_FindMalleableVar("bullet_penetrationMinFxDist");

		while (true) {
			if (trace.depthSurfaceType)
			{
				penetrationDepth = penetrationDepthTable[29 * weapon->penetrateType + trace.depthSurfaceType];
			}
			else {
				penetrationDepth = 0.f;
			}

			if (ignoreEnt->nextState.eType == ET_PLAYER && (cgs->bgs.clientinfo[ignoreEnt->nextState.clientNum].perks & 0x20) != 0)
			{
				penetrationDepth = Dvar_FindMalleableVar("perk_bulletPenetrationMultiplier")->current.value * penetrationDepth;
			}
			if (penetrationDepth <= 0.0)
			{
				return trace;
			}

			hitpos = trace.hitPos;
			if (!BG_AdvanceTrace(&trace, fireParams, 0.135000f))
			{
				return trace;
			}

			traceHit = BulletTrace(&trace, fireParams, ignoreEnt, trace.depthSurfaceType);
			memcpy(&bpp, fireParams, sizeof(BulletFireParams));
			bpp.dir[0] = -fireParams->dir[0];
			bpp.dir[1] = -fireParams->dir[1];
			bpp.dir[2] = -fireParams->dir[2];
			bpp.start[0] = fireParams->end[0];
			bpp.start[1] = fireParams->end[1];
			bpp.start[2] = fireParams->end[2];
			bpp.end[0] = bpp.dir[0] * 0.009999f + hitpos.x;
			bpp.end[1] = bpp.dir[1] * 0.009999f + hitpos.y;
			bpp.end[2] = 0.009999f * bpp.dir[2] + hitpos.z;
			memcpy(&br, &trace, sizeof(BulletTraceResults));
			br.trace.normal[0] = -br.trace.normal[0];
			br.trace.normal[1] = -br.trace.normal[1];
			inverseZ = -br.trace.normal[2];
			br.trace.normal[2] = inverseZ;

			if (traceHit)
			{
				inverseZ = 0.0099999998;
				BG_AdvanceTrace(&br, &bpp, 0.0099999998);
			}

			traceHit = BulletTrace(&br, &bpp, ignoreEnt, br.depthSurfaceType);

			if (traceHit && br.trace.allsolid || trace.trace.startsolid && br.trace.startsolid) {
				old_solid = true;
				new_solid = true;
			}
			else {
				old_solid = false;
				new_solid = false;
			}

			if (!traceHit && !old_solid)
			{
				break;
			}

			if (br.trace.hitType == TRACE_HITTYPE_DYNENT_MODEL || br.trace.hitType == TRACE_HITTYPE_DYNENT_BRUSH)
				hitId = 1022;
			else if (br.trace.hitType == TRACE_HITTYPE_ENTITY)
				hitId = br.trace.hitId;
			else
				hitId = 1023;

			if (old_solid)
			{
				fireEnd = bpp.end;
				fireStart = bpp.start;
			}
			else {
				fireEnd = hitpos;
				fireStart = br.hitPos;
			}

			//normalZ = inverseZ;

			normalZ = (fvec3(fireEnd) - fvec3(fireStart)).mag();

			if (normalZ < 1.0)
			{
				normalZ = 1.0;
			}

			if (traceHit) {

				if (ignoreEnt->nextState.eType == ET_PLAYER && (cgs->bgs.clientinfo[ignoreEnt->nextState.clientNum].perks & 0x20) != 0)
				{
					surfacePenetrationDepth = BG_GetSurfacePenetrationDepth(br.depthSurfaceType, weapon) * Dvar_FindMalleableVar("perk_bulletPenetrationMultiplier")->current.value;
				}
				else
				{
					surfacePenetrationDepth = BG_GetSurfacePenetrationDepth(br.depthSurfaceType, weapon);
				}

				//std::cout << "depth: " << surfacePenetrationDepth << '\n';

				penetrationDepth = std::min(penetrationDepth, surfacePenetrationDepth);
				if (penetrationDepth <= 0.0)
				{
					return br;
				}
			}

			surfacePenetrationDepth = fireParams->damageMultiplier - normalZ / penetrationDepth;
			fireParams->damageMultiplier = surfacePenetrationDepth;
			if (surfacePenetrationDepth <= 0.0)
			{
				return br;
			}

			if (!new_solid && weapon->weapType == WEAPTYPE_BULLET)
			{
				surfacePenetrationDepth = bullet_penetrationMinFxDist->current.value * bullet_penetrationMinFxDist->current.value;
				if (surfacePenetrationDepth < (fvec3(br.hitPos) - fvec3(trace.hitPos)).MagSq()) {

					surfFlags = trace.trace.surfaceFlags;

					if (!traceHit)
						return br;
				}

			}
			hit:
			if (traceHit) {
				if (++numHits < 5)
					continue;
			}
			return br;
		}

		if (!traceHit)
			return br;

		if (trace.trace.allsolid) {
			goto hit;
		}

		surfacePenetrationDepth = bullet_penetrationMinFxDist->current.value * bullet_penetrationMinFxDist->current.value;
		if (surfacePenetrationDepth < (hitpos - fvec3(trace.hitPos)).MagSq() || weapon->weapType) {
			goto hit;
		}


		goto hit;
	}

	return trace;
}