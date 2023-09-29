#pragma once

#include "pch.hpp"

bool PM_DisallowMouse(const std::optional<bool>& state=std::nullopt);

void PM_WalkMove(pmove_t* pm, pml_t* pml);
void PM_AirMove(pmove_t* pm, pml_t* pml);
void PM_UpdateViewAngles(playerState_s* ps, float msec, usercmd_s* cmd, char handler);
void Mantle_Check(pmove_t* pm, pml_t* pml);

inline void(__cdecl* PM_CheckLadderMove_)(pmove_t* pm, pml_t* pml) = ((void(__cdecl*)(pmove_t*, pml_t*))0x413A60);
inline void(__cdecl* PM_LadderMove_)(pmove_t* pm, pml_t* pml) = ((void(__cdecl*)(pmove_t*, pml_t*))0x413DD0);
inline void(__cdecl* PM_AdjustAimSpreadScale_)(pmove_t* pm, pml_t* pml) = ((void(__cdecl*)(pmove_t*, pml_t*))0x418870);
inline void(*PM_UpdateSprint_)(pmove_t* pm) = ((void(__cdecl*)(pmove_t*))0x40E6A0);
//inline void(*PM_UpdateAimDownSightFlag_)(pmove_t* pm, pml_t* pml) = ((void(__cdecl*)(pmove_t*, pml_t*))0x416E60);
//void(*PM_Footsteps_)(pmove_t* pm, pml_t* pml) = ((void(__cdecl*)(pmove_t*, pml_t*))0x412180);
void PM_OverBounce(pmove_t* pm, pml_t* pml);

void PM_Footsteps_(pmove_t* pm, pml_t* pml);
void PM_GroundTrace_(pmove_t* pm, pml_t* pml);
void PM_DropTimers(playerState_s* ps, pml_t* pml);
void PM_UpdatePronePitch(pml_t* pml, pmove_t* pm);
void Mantle_Move(playerState_s* ps, pmove_t* pm, pml_t* pml);
void PM_CheckDuck_(pmove_t* pm, pml_t* pml);
void PM_UpdatePlayerWalkingFlag_(pmove_t* pm);
void PM_Weapon(pmove_t* pm, pml_t* pml);
void _PM_UpdateViewAngles(playerState_s* ps, float msec, usercmd_s* cmd, char handler);

inline void PM_UpdateAimDownSightFlag_(pmove_t* pm, pml_t* pml)
{
	__asm
	{
		mov eax, pml;
		mov ebx, pm;
		push eax;
		push ebx;
		mov esi, 0x416E60;
		call esi;
		add esp, 0x8;
	}
}

void Sys_SnapVector(float* v);


pmove_t PM_Create(playerState_s* ps, usercmd_s* cmd, usercmd_s* oldcmd);
pml_t PML_Create(pmove_t* pm, int FPS);