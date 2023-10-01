#include "pch.hpp"

void CL_FixServerTime(usercmd_s* cmd)
{
	static dvar_s* com_maxfps = Dvar_FindMalleableVar("com_maxfps");
	static int first_time = cmd->serverTime;
	static int stime = 0;

	if (abs(cmd->serverTime - first_time) > 100)
		first_time = cmd->serverTime;
	cmd->serverTime = first_time;
	clients->serverTime = cmd->serverTime;
	first_time += 1000 / com_maxfps->current.integer == 0 ? 1 : com_maxfps->current.integer;
}
void CL_FinishMove(usercmd_s* cmd)
{

	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_CL_FINISHMOVE);
	detour_func.cast_call<void(*)(usercmd_s*)>(cmd);
	static MovementRecorder& mr = MovementRecorder::getInstance();
	static Elebot& elebot = Elebot::getInstance();
	CL_FixServerTime(cmd);



	mr.OnUserCmd(cmd);

	elebot.move(cmd);
	elebot.do_playback(cmd);


	lineup_testing_func(cmd);

	return;

}

void CL_WritePacket()
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_CL_WRITEPACKET);

	if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
		clients->snap.ps.delta_angles[YAW] = 10;

		std::cout << "yea!\n";
	}

	detour_func.cast_call<void(*)()>();


}
void CL_ParseSnapshot2(msg_t* msg)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_CL_PARSESNAPSHOT);

	if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		std::cout << "yo!\n";

	

	void* fnc = detour_func.get_ptr();

	__asm
	{
		mov eax, msg;
		call fnc;
		retn;
	}

	//return detour_func.cast_call<void(*)(msg_t*)>(msg);
}
__declspec(naked) void CL_ParseSnapshot(msg_t* msg) 
{
	__asm
	{
		push eax;
		call CL_ParseSnapshot2;
		add esp, 0x4;
		retn;
	}
}