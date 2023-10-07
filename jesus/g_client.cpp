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