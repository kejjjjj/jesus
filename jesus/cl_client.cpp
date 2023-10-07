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

	CL_FixServerTime(cmd);


	mr.OnUserCmd(cmd);

	//elebot.move(cmd);
	//elebot.do_playback(cmd);

	CL_MonitorEvents();

	static dvar_s* kej_bhop = Dvar_FindMalleableVar("kej_bhop");

	if (kej_bhop && kej_bhop->current.enabled && (cmd->buttons & cmdEnums::jump) != 0) {
			
		if (cmd->serverTime - cgs->predictedPlayerState.jumpTime >= 500 
			&& cgs->predictedPlayerState.groundEntityNum == 1023 
			&& CG_GetDistanceToGround(&cgs->predictedPlayerState) < 100)
			cmd->buttons -= cmdEnums::jump;

		else if(cmd->serverTime - cgs->predictedPlayerState.jumpTime < 500 && cgs->predictedPlayerState.groundEntityNum == 1022)
			cmd->buttons -= cmdEnums::jump;


		
	}

	static dvar_s* kej_easyBounces = Dvar_FindMalleableVar("kej_easyBounces");

	if (kej_easyBounces && kej_easyBounces->current.enabled) {
		static bool playback_active = false;
		static bool stop_predicting = false;
		static std::list<playback_cmd>::iterator pb_it;
		static std::list<playback_cmd>::iterator pb_end;
		static std::optional<std::list<playback_cmd>> results;

		bool bplayback = test_playback && test_playback->isPlayback();

		if (stop_predicting == false && !mr.playback)
			results = RealTimePrediction(&cgs->predictedPlayerState, cmd);

		if (results) {
			pb_it = results.value().begin();
			test_playback = new Playback(results.value());

			test_playback->StartPlayback(cmd->serverTime, &cgs->predictedPlayerState, cmd, *pb_it, false);
			stop_predicting = true;
			results = std::nullopt;


			auto current = test_playback->CurrentCmd();

			if (!current)
				return;

			float dist = current->origin.dist(clients->cgameOrigin);

			//Com_Printf("start dist: %.6f\n", dist);
			//bplayback = true;
		}

		if (bplayback) {
			test_playback->doPlayback(cmd);

			if (!test_playback->isPlayback()) {
				//Com_Printf("bunce!\n");
				delete test_playback;
				test_playback = 0;
			}


		}

		if (cgs->predictedPlayerState.groundEntityNum == 1022 && !bplayback)
			stop_predicting = false;
	}
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

	if (GetAsyncKeyState(VK_NUMPAD0) & 1)
		std::cout << health << ", " << mr.playbacks.size() << '\n';

	if (health && mr.playbacks.empty()) {

		if (mr.playbacks_loaded == false) {
			mr.OnLoadFromMemory(&cgs->predictedPlayerState);

		}

	}


}