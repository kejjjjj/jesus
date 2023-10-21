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

	if (CG_CalcPlayerHealth(&cgs->predictedPlayerState) == NULL)
		return;

	static MovementRecorder& mr = MovementRecorder::getInstance();
	static Elebot& elebot = Elebot::getInstance();
	static BouncePrediction& bp = BouncePrediction::getInstance();
	static entities_s& entities = entities_s::get();

	CL_FixServerTime(cmd);
	bp.PredictBounce(cmd);
	mr.OnUserCmd(cmd);
	elebot.move(cmd);
	CL_MonitorEvents();

	entities.update_all(clients->snap.numClients);

	static dvar_s* kej_bhop = Dvar_FindMalleableVar("kej_bhop");

	if (kej_bhop && kej_bhop->current.enabled && (cmd->buttons & cmdEnums::jump) != 0) {
		usercmd_s* oldcmd = CL_GetUserCmd(clients->cmdNumber - 1);
		if ((cmd->buttons & cmdEnums::jump) != 0 && (oldcmd->buttons & cmdEnums::jump) != 0)
			cmd->buttons -= cmdEnums::jump;
	}

	M_MovementCheats(cmd);

	return;

}

void CL_WritePacket()
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_CL_WRITEPACKET);
	detour_func.cast_call<void(*)()>();


}
void CL_ParseSnapshot2(msg_t* msg)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_CL_PARSESNAPSHOT);


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

void CL_MonitorEvents()
{
	static float old_health = 0.f;
	float health = CG_CalcPlayerHealth(&cgs->predictedPlayerState);
	
	static MovementRecorder& mr = MovementRecorder::getInstance();

	if (health && mr.playbacks.empty()) {

		if (mr.playbacks_loaded == false) {
			mr.OnLoadFromMemory(&cgs->predictedPlayerState);

		}

	}


}