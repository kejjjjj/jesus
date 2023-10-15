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

	if (surfaceType)
	{
		result = penetrationDepthTable[29 * weapon->penetrateType + surfaceType];
	}
	else
	{
		result = 0.0;
	}
	return result;
}
void G_CalculateBulletPenetration(BulletFireParams* bfp, WeaponDef* weapon, vec3_t start, vec3_t end, int ignoreId, int clientNum, vec3_t forward, vec3_t right, vec3_t up)
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

	//fvec3 angles = fvec3(dir).toangles();

	//vec3_t forward{ 1,1,1 };
	//vec3_t right{ 1,1,1 };
	//vec3_t up{ 1,1,1 };

	//AngleVectors(dir, forward, right, up);

	
	VectorNormalize(dir);
	VectorCopy(dir, bfp->dir);

	
	fvec3 tracerStart = rg->viewOrg;

	////std::cout << "tracerStart: " << tracerStart << '\n';

	float len = std::clamp(sqrtf(tracerStart.mag()), 0.f, 255.f);

	float maxSpread = weapon->hipSpreadStandMax;
	float spread = len / 255.f;

	spread = (maxSpread - weapon->fAdsSpread) * spread + weapon->fAdsSpread;

	for (int shot = 0; shot < shotCount; shot++) {

		//VectorCopy(start, _start);
		VectorCopy(start, bfp->start);
		VectorCopy(forward, bfp->dir);



		CG_BulletEndpos(up, bfp->end, shot + cgs->predictedPlayerState.commandTime,
			spread, bfp->start, bfp->dir, forward, right, damageRange);


		bfp->damageMultiplier = 1.f;

		_asm
		{
			mov al, 1;
			push al;
			mov edi, ignoreEnt;
			push start;
			push weapon;
			mov ebx, bfp;
			push ebx;
			push clientNum;
			mov esi, 0x455A80
			call esi;
			add esp, 0x14;
		}
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