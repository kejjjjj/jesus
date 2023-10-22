#pragma once

#include "pch.hpp"

int BG_FindWeaponIndexForName(const char* name);

void __cdecl G_SelectWeaponIndex(int clientNum, int iWeaponIndex);
//std::list<WeaponDef*> G_GetWeaponsList();
uint32_t CG_SelectWeaponIndex(uint32_t weaponIndex, int localClientNum);
void CG_GuessSpreadForWeapon(float* maxSpread, WeaponDef* weapDef, centity_s* cent, float* minSpread);