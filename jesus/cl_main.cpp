#include "pch.hpp"

void CL_Disconnect()
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_CL_DISCONNECT);

	if (clientUI->connectionState != CA_DISCONNECTED) { //gets called in the loading screen in 1.7
		
		MovementRecorder::getInstance().OnDisconnect();

	}

	detour_func.cast_call<void(*)()>();
}
void SV_Map(void* a1)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_SV_MAP);

	detour_func.cast_call<void(*)(void*)>(a1);

	const dvar_s* sv_running = Dvar_FindMalleableVar("sv_running");
	const char* mapname = (*(sv_cmd_args->argv[sv_cmd_args->nesting] + 1));

	if (!sv_running->current.enabled) {
		Com_Printf(CON_CHANNEL_CONSOLEONLY, "^1failed to [%s] because '%s' is missing\n", *(sv_cmd_args->argv[sv_cmd_args->nesting] + 0), *(sv_cmd_args->argv[sv_cmd_args->nesting] + 1));
		return;
	}

	decltype(auto) r = MovementRecorder::getInstance();

	r.LoadRecordings(mapname);

	return;

}
void LoadMapLoadscreen(char* mapname)
{
	char* mapName = 0;
	__asm mov mapName, eax;

	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_LOADMAP_LOADSCREEN);
	
	std::cout << "loading map: " << mapname << '\n';

	void* org = detour_func.get_ptr();

	__asm
	{
		mov eax, mapname;
		call dword ptr org;
	}

	decltype(auto) r = MovementRecorder::getInstance();


	r.LoadRecordings(mapname);


	return;
	//detour_func.cast_call<void(*)(char*)>(mapname);
}