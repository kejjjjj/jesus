#include "pch.hpp"

int BG_FindWeaponIndexForName(const char* name)
{
	return engine_call<int>(false, 0x416610, name);
}

void __cdecl G_SelectWeaponIndex(int clientNum, int iWeaponIndex)
{
	const DWORD G_SelectWeaponIndex_f = 0x4EA470;
	__asm
	{
		mov eax, iWeaponIndex;
		mov esi, clientNum;
		call G_SelectWeaponIndex_f;
	}

}
//std::list<WeaponDef*> G_GetWeaponsList()
//{
//	std::list<WeaponDef*> weapons;
//
//
//	for (int i = 1; i < *bg_lastParsedWeaponIndex + 1; i++) {
//
//		if (((1 << (i & 0x1F)) & clients->snap.ps.weapons[i >> 5]) != 0) {
//			weapons.push_back(BG_WeaponNames[i]);
//		}
//
//	}
//
//	return weapons;
//}
uint32_t CG_SelectWeaponIndex(uint32_t weaponIndex, int localClientNum)
{
	uint32_t out = 0;
	__asm
	{
		mov eax, weaponIndex;
		mov ecx, localClientNum;
		mov esi, 0x458F10;
		call esi;
		mov out, eax;

	}

	return out;
}

void CG_GuessSpreadForWeapon(float* maxSpread, WeaponDef* weapDef, centity_s* cent, float* minSpread)
{
	static DWORD fnc = 0x456250;
	__asm
	{
		push minSpread;
		push cent;
		mov esi, weapDef;
		mov edi, maxSpread;
		call fnc;
		add esp, 0x8;
	}

}