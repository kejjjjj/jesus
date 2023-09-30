
#include "pch.hpp"
bool PM_DisallowMouse(const std::optional<bool>& state)
{
	static bool bState = false;

	if (state)
		bState = state.value();

	return bState;
}
void PM_AirMove(pmove_t* pm, pml_t* pml)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_PM_AIRMOVE);

	memcpy(pm_glob, pm, sizeof(pmove_t));
	memcpy(pml_glob, pml, sizeof(pml_t));


	return detour_func.cast_call<void(__cdecl*)(pmove_t* pm, pml_t* pml)>(pm, pml);
}
void PM_WalkMove(pmove_t* pm, pml_t* pml)
{
	memcpy(pm_glob, pm, sizeof(pmove_t));
	memcpy(pml_glob, pml, sizeof(pml_t));

	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_PM_WALKMOVE);

	return detour_func.cast_call<void(__cdecl*)(pmove_t * pm, pml_t * pml)>(pm, pml);
}

void __cdecl UpdateViewAngles(float msec, char handler)
{
	usercmd_s* cmd;
	playerState_s* ps;

	__asm retn;
	__asm mov ps, eax;
	__asm mov cmd, esi;
	
	double v4; // st6
	int pm_type; // eax
	double result; // st7
	int pm_flags; // eax
	__int16 v9; // fps
	bool v10; // c0
	char v11; // c2
	bool v12; // c3
	int _pm_type; // eax
	float oldViewYawa; // [esp+20h] [ebp-4h]
	float oldViewYaw; // [esp+20h] [ebp-4h]

	
	pm_type = ps->pm_type;
	if (pm_type != PM_INTERMISSION)
	{
		if (pm_type >= PM_DEAD)
		{
			if (ps->stats[STAT_DEAD_YAW] == 999)
			{
				oldViewYawa = cmd->angles[1] * 0.0054931640625 + ps->delta_angles[1];
				ps->stats[STAT_DEAD_YAW] = (AngleNormalize360(oldViewYawa) * 0.0054931640625);
			}

			result = msec;
			
			auto ah = (&pmoveHandlers)[2 * handler];

			__asm
			{
				mov eax, cmd;
				mov edi, ps;
				push ah;
				fstp[msec];
				push ecx;
				mov esi, 0x412590; //PM_UpdateLean
				call esi;
				add esp, 0x8;

			}

			//PM_UpdateLean(cmd, ps, msec, (&pmoveHandlers)[2 * handler]);
			return;
		}
		result = ps->viewangles[1];
		oldViewYaw = ps->viewangles[1];


		__asm
		{
			mov edi, ps;
			push cmd;
			mov esi, 0x412980; //PM_UpdateViewAngles_Clamp
			call esi;
			add esp, 0x4;
		}

		//if (GetAsyncKeyState(VK_NUMPAD3) & 1 ) {
		//	Com_Printf(CON_CHANNEL_OBITUARY, "yaw: ^1%.2f\n", ps->viewangles[YAW]);
		//	float delta = (ps->viewangles[YAW] -  69);

		//	Com_Printf(CON_CHANNEL_OBITUARY, "angle delta: ^2%.2f\n", delta);

		//	ps->viewangles[YAW] += delta;
		//	cmd->angles[YAW] += ANGLE2SHORT(delta);
		//	ps->delta_angles[YAW] -= (delta);

		//}

	}


	return;

	//return find_hook(hookEnums_e::HOOK_PM_UPDATE_VIEWANGLES).cast_call<void(*)(playerState_s*, float, usercmd_s*, char)>(ps, msec, cmd, handler);

}
void WTF(playerState_s* ps, float msec, usercmd_s* cmd, char handler) 
{


	double v4; // st6
	int pm_type; // eax
	double result; // st7
	int pm_flags; // eax
	__int16 v9; // fps
	bool v10; // c0
	char v11; // c2
	bool v12; // c3
	int _pm_type; // eax
	float oldViewYawa; // [esp+20h] [ebp-4h]
	float oldViewYaw; // [esp+20h] [ebp-4h]

	const bool disallowMouse = PM_DisallowMouse();
	
	float oldYaw = ps->viewangles[YAW];
	float oldPitch = ps->viewangles[PITCH];
	float oldCmdYaw = cmd->angles[YAW];
	float oldDeltaYaw = ps->delta_angles[YAW];


	//if (PM_DisallowMouse())
	//	return;

	msec = cgs->frametime;


	pm_type = ps->pm_type;
	if (pm_type != PM_INTERMISSION)
	{
		if (pm_type >= PM_DEAD)
		{
			if (ps->stats[STAT_DEAD_YAW] == 999)
			{
				oldViewYawa = cmd->angles[1] * 0.0054931640625 + ps->delta_angles[1];
				ps->stats[STAT_DEAD_YAW] = (AngleNormalize360(oldViewYawa) * 0.0054931640625);
			}

			result = msec;

			auto ah = (&pmoveHandlers)[2 * handler];

			__asm
			{
				mov eax, cmd;
				mov edi, ps;
				push ah;
				fstp[msec];
				push ecx;
				mov esi, 0x412590; //PM_UpdateLean
				call esi;
				add esp, 0x8;

			}

			//PM_UpdateLean(cmd, ps, msec, (&pmoveHandlers)[2 * handler]);
			return;
		}
		result = ps->viewangles[1];
		oldViewYaw = ps->viewangles[1];


		__asm
		{
			mov edi, ps;
			push cmd;
			mov esi, 0x412980; //PM_UpdateViewAngles_Clamp
			call esi;
			add esp, 0x4;
		}

		//if (GetAsyncKeyState(VK_NUMPAD3) & 1 ) {
		//	Com_Printf(CON_CHANNEL_OBITUARY, "yaw: ^1%.2f\n", ps->viewangles[YAW]);
		//	float delta = (ps->viewangles[YAW] -  69);

		//	Com_Printf(CON_CHANNEL_OBITUARY, "angle delta: ^2%.2f\n", delta);

		//	ps->viewangles[YAW] += delta;
		//	cmd->angles[YAW] += ANGLE2SHORT(delta);
		//	ps->delta_angles[YAW] -= (delta);

		//}

	}


	if (PM_DisallowMouse()) {
		ps->viewangles[YAW] = oldYaw;
		ps->viewangles[PITCH] = oldPitch;

		//ps->delta_angles[YAW] = oldDeltaYaw;
		//cmd->angles[YAW] = oldCmdYaw;
		return;
	}


	return;
}
__declspec(naked) void PM_UpdateViewAngles(playerState_s* ps, float msec, usercmd_s* cmd, char handler)
{
	static float yup;
	__asm
	{
		sub esp, 0x8;
		push ebx;
		mov ebx, dword ptr[esp + 0x18]; //handler
		push ebp;
		mov ebp, [esp + 0x18]; //cmd
		push esi;
		push edi;

		mov edi, eax;

		push ebx; //handler
		push ebp;
		push[esp + 0x60-0x8]; //msec
		push edi; //ps
		

		call WTF;
		add esp, 0x10;

		pop edi;
		pop esi;
		pop ebp;
		pop ebx;
		add esp, 8;

		retn;
	}
}
void Sys_SnapVector(float* v)
{
	int i;
	float f;

	f = *v;
	__asm	fld		f;
	__asm	fistp	i;
	*v = i;
	v++;
	f = *v;
	__asm	fld		f;
	__asm	fistp	i;
	*v = i;
	v++;
	f = *v;
	__asm	fld		f;
	__asm	fistp	i;
	*v = i;
	/*
	*v = fastftol(*v);
	v++;
	*v = fastftol(*v);
	v++;
	*v = fastftol(*v);
	*/
}
bool Mantle_FindMantleSurface(pmove_t* pm, trace_t* trace, float* normal, float* fwd)
{
	dvar_s* mantle_check_radius = Dvar_FindMalleableVar("mantle_check_radius");
	dvar_s* mantle_check_range = Dvar_FindMalleableVar("mantle_check_range");
	dvar_s* mantle_debug = Dvar_FindMalleableVar("mantle_debug");
	dvar_s* mantle_check_angle = Dvar_FindMalleableVar("mantle_check_angle");

	float* v4; // ecx
	playerState_s* v7; // edi
	int v8; // ecx
	char result; // al
	float angles[3]; // [esp+18h] [ebp-3Ch] BYREF
	float end[3]; // [esp+24h] [ebp-30h] BYREF
	float maxs[3]; // [esp+30h] [ebp-24h] BYREF
	float mins[3]; // [esp+3Ch] [ebp-18h] BYREF
	float start[3]; // [esp+48h] [ebp-Ch] BYREF
	float mantle_range; // [esp+58h] [ebp+4h]
	float mins_v; // [esp+5Ch] [ebp+8h]
	float prone_mins; // [esp+5Ch] [ebp+8h]
	float tracec; // [esp+5Ch] [ebp+8h]
	float traced; // [esp+5Ch] [ebp+8h]

	mins_v = -mantle_check_radius->current.value;
	mins[0] = mins_v;
	mins[1] = mins_v;
	v7 = pm->ps;
	mins[2] = 0.0;
	maxs[0] = mantle_check_radius->current.value;
	maxs[1] = mantle_check_radius->current.value;
	maxs[2] = 70.0;
	prone_mins = 15.0 - mantle_check_radius->current.value;
	mantle_range = mantle_check_range->current.value + prone_mins;
	angles[0] = *fwd;
	angles[1] = fwd[1];
	angles[2] = 0.0;
	VectorNormalize(angles);
	v8 = pm->handler;
	tracec = -prone_mins;
	start[0] = tracec * angles[0] + v7->origin[0];
	start[1] = angles[1] * tracec + v7->origin[1];
	start[2] = tracec * angles[2] + v7->origin[2];
	end[0] = angles[0] * mantle_range + v7->origin[0];
	end[1] = angles[1] * mantle_range + v7->origin[1];
	end[2] = angles[2] * mantle_range + v7->origin[2];

	pmoveHandler_t* handler = reinterpret_cast<pmoveHandler_t*>(0x729448);

	handler->trace(trace, start, mins, maxs, end, v7->clientNum, 0x1000000);

	if (trace->startsolid || trace->allsolid)
	{
		if (mantle_debug->current.enabled)
			Com_Printf(CON_CHANNEL_PLAYERWEAP, "%s\n", "Mantle Failed: Mantle brush is too thick");
		return 0;
	}
	if (1.0 == trace->fraction)
	{
		if (mantle_debug->current.enabled)
			Com_Printf(CON_CHANNEL_PLAYERWEAP, "%s\n", "Mantle Failed: No mantle surface found");
		return 0;
	}
	if ((trace->surfaceFlags & 0x6000000) != 0)
	{
		*normal = -trace->normal[0];
		normal[1] = -trace->normal[1];
		normal[2] = 0.0;
		if (VectorNormalize(normal) >= 0.00009999999747378752)
		{
			traced = normal[1] * angles[1] + *normal * angles[0] + normal[2] * angles[2];
			if (mantle_check_angle->current.value >= acos(traced) * 57.2957763671875)
			{
				//Com_Printf(CON_CHANNEL_SUBTITLE, "Mantle range!\n");
				result = 1;
			}
			else
			{
				Com_Printf(CON_CHANNEL_CONSOLEONLY, "Mantle Failed: Player is not facing mantle surface\n");
				result = 0;
			}
		}
		else
		{
			Com_Printf(CON_CHANNEL_CONSOLEONLY, "Mantle Failed: Mantle surface has vertical normal\n");
			result = 0;
		}
	}
	else
	{
		Com_Printf(CON_CHANNEL_CONSOLEONLY, "Mantle Failed: No mantle surface with MANTLE_ON or MANTLE_OVER found\n");
		result = 0;
	}

	return result;
}
bool Mantle_CheckLedge(MantleResults* r, pmove_t* pm, float height)
{
	char res{};
	__asm
	{
		//fld height;
		push height;
		mov ecx, pm;
		mov eax, r;
		mov esi, 0x408660;
		call esi;
		add esp, 0x4;
		mov res, al;
	}
	return res;

	//return res;
}
void Mantle_Check(pmove_t* pm, pml_t* pml)
{
	//	return Mantle_Check_f(pm, pml);

		//__asm mov pm, edi;
		//__asm mov pml, eax;
	playerState_s* ps;
	float normal[3]; // [esp+8h] [ebp-6Ch] BYREF
	trace_t trace;
	MantleResults mantle;

	pm->ps->mantleState.flags &= 0xFFFFFFEF;

	static dvar_s* mantle_enable = Dvar_FindMalleableVar("mantle_enable");

	if (mantle_enable->current.enabled) {

		ps = pm->ps;
		ps->mantleState.flags &= 0xFFFFFFF7;

		if (ps->pm_type < PM_DEAD)
		{
			if ((ps->pm_flags & PMF_MANTLE) != 0)
			{

			}
			else if ((ps->eFlags & 0xC) != 0) {

			}
			else
			{
				bool result = Mantle_FindMantleSurface(pm, &trace, normal, pml->forward);

				if (result)
				{
					memset(&mantle, 0, sizeof(MantleResults));
					mantle.dir[0] = normal[0];
					mantle.dir[1] = normal[1];
					mantle.dir[2] = normal[2];
					mantle.startPos[0] = ps->origin[0];
					mantle.startPos[1] = ps->origin[1];
					mantle.startPos[2] = ps->origin[2];
					if ((trace.surfaceFlags & 0x4000000) != 0)
						mantle.flags |= 1u;

					char result = Mantle_CheckLedge(&mantle, pm, 60.f);
					if (!result) {
						char result = Mantle_CheckLedge(&mantle, pm, 40.f);
						if (!result)
							char result = Mantle_CheckLedge(&mantle, pm, 20.f);
					}
					//if (result) {
					//	Com_Printf(CON_CHANNEL_SUBTITLE, "very good!\n");
					//}
				}

			}

		}

	}

	return;
}

void PM_Footsteps_(pmove_t* pm, pml_t* pml)
{
	__asm
	{
		push pml;
		mov eax, pm;
		mov esi, 0x412180;
		call esi;
		add esp, 0x4;
	}
}
void PM_GroundTrace_(pmove_t* pm, pml_t* pml)
{

	__asm
	{
		push pml;
		push pm;
		mov esi, 0x00410660;
		call esi;
		add esp, 0x8;
	}

}
void PM_DropTimers(playerState_s* ps, pml_t* pml)
{
	uintptr_t fnc = 0x00412510;
	__asm
	{
		mov eax, ps;
		mov esi, pml;
		call fnc;
	}
}
void PM_UpdatePronePitch(pml_t* pml, pmove_t* pm)
{
	__asm
	{
		push pm;
		mov edi, pml;
		mov esi, 0x4136E0;
		call esi;
		add esp, 0x4;
	}
}
void Mantle_Move(playerState_s* ps, pmove_t* pm, pml_t* pml)
{
	__asm
	{

		mov edi, ps;
		push pml;
		push pm;
		mov esi, 0x409000;
		call esi;
		add esp, 0x8;
	}
}
void PM_CheckDuck_(pmove_t* pm, pml_t* pml)
{
	__asm
	{
		lea ecx, pml;
		push ecx;
		mov edi, pm;
		mov esi, 0x410F70;
		call esi;
		add esp, 0x4;
	}
}
void PM_UpdatePlayerWalkingFlag_(pmove_t* pm) {
	__asm
	{
		mov ecx, pm;
		mov esi, 0x413A10;
		call esi;

	}
}
void PM_Weapon(pmove_t* pm, pml_t* pml)
{
	__asm
	{
		mov eax, pml;
		mov ecx, pm;
		mov esi, 0x0041A470;
		call esi;
	}
}
void _PM_UpdateViewAngles(playerState_s* ps, float msec, usercmd_s* cmd, char handler) {

	__asm
	{

		movzx eax, handler;
		mov esi, cmd;
		push eax;
		push esi;
		push msec;
		mov eax, ps;
		mov esi, 0x413580;
		call esi;
		add esp, 0xC;
	}

}
void PM_OverBounce(pmove_t* pm, pml_t* pml)
{
	vec3_t move;

	move[0] = pm->ps->origin[0] - pml->previous_origin[0];
	move[1] = pm->ps->origin[1] - pml->previous_origin[1];
	move[2] = pm->ps->origin[2] - pml->previous_origin[2];

	float dot = move[2] * move[2] + move[1] * move[1] + move[0] * move[0];
	float dot_div_frametime = dot / (pml->frametime * pml->frametime);
	float dot_speed = pm->ps->velocity[2] * pm->ps->velocity[2] + pm->ps->velocity[1] * pm->ps->velocity[1] + pm->ps->velocity[0] * pm->ps->velocity[0];

	if (dot_speed * 0.25 > dot_div_frametime)
	{
		//Com_Printf(CON_CHANNEL_OBITUARY, "possible overbounce!\n");
		float inGameFramesPerSecond = 1.0 / pml->frametime;
		pm->ps->velocity[0] = inGameFramesPerSecond * move[0];
		pm->ps->velocity[1] = inGameFramesPerSecond * move[1];
		pm->ps->velocity[2] = inGameFramesPerSecond * move[2];
	}

	float clampedFrametime = std::clamp(pml->frametime, 0.f, 1.f);

	float diffX = pm->ps->velocity[0] - pm->ps->oldVelocity[0];
	float diffY = pm->ps->velocity[1] - pm->ps->oldVelocity[1];

	float frameX = clampedFrametime * diffX;
	float frameY = clampedFrametime * diffY;

	pm->ps->oldVelocity[0] = pm->ps->oldVelocity[0] + frameX;
	pm->ps->oldVelocity[1] = pm->ps->oldVelocity[1] + frameY;


	return;
}

pmove_t PM_Create(playerState_s* ps, usercmd_s* cmd, usercmd_s* oldcmd)
{
	std::unordered_map<int, int> stance = { {60, 70}, {40, 50}, {11, 30} };

	pmove_t pm{};

	
	pm.ps = ps;
	pm.cmd = *cmd;
	pm.oldcmd = *oldcmd;

	pm.mins[0] = -15;
	pm.mins[1] = -15;
	pm.mins[2] = 0;
	pm.maxs[0] = 15;
	pm.maxs[1] = 15;
	pm.maxs[2] = stance.find(int(ps->viewHeightCurrent))->second;
	pm.tracemask = 42008593;
	pm.handler = 1;

	return pm;
	
}
pml_t PML_Create(pmove_t* pm, int FPS)
{
	pml_t pml{};

	FPS = std::clamp(FPS, 1, 1000);

	pml.msec = 1000 / FPS;
	pml.frametime = float(pml.msec) / 1000.f;
	VectorCopy(pm->ps->origin, pml.previous_origin);
	VectorCopy(pm->ps->velocity, pml.previous_velocity);

	AngleVectors(pm->ps->viewangles, pml.forward, pml.right, pml.up);
	
	hook::write_addr(0x537D10, "\xC3", 1);

	engine_call<void>(false, 0x410660, pm, &pml); //call groundtrace to fill pml

	hook::write_addr(0x537D10, "\x81", 1);


	return pml;
}