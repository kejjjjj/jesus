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
//void test_spread(usercmd_s* cmd)
//{
//	static int next_cmd = cmd->serverTime + 8;
//
//	if(cmd->serverTime >= next_cmd)
//		next_cmd = cmd->serverTime + 8;
//
//	if ((cmd->buttons & cmdEnums::fire) == 0 || cgs->predictedPlayerState.weaponDelay != NULL)
//		return;
//
//	decltype(auto) spread_data = spreadData::get();
//	vec3_t end;
//	vec3_t dir;
//	fvec3 start = cgs->predictedPlayerState.origin;
//	start.z += cgs->predictedPlayerState.viewHeightCurrent;
//
//	float maxSpread, minSpread;
//
//
//	WeaponDef* weapon = BG_WeaponNames[cgs->predictedPlayerState.weapon];
//	
//	CG_GuessSpreadForWeapon(&maxSpread, weapon, &centity[cgs->clientNum], &minSpread);
//	
//	float damageRange = 8192.f;
//
//	if (weapon->weapClass == WEAPCLASS_SPREAD) {
//		damageRange = weapon->fMinDamageRange;
//	}
//
//	float spread = std::min<float>(start.mag(), 255.f) / 255.f;
//
//	spread = (maxSpread - weapon->fAdsSpread) * spread + weapon->fAdsSpread;
//
//	CG_BulletEndpos(cgs->refdef.viewaxis[2], end, cmd->serverTime+8, spread, start, dir, cgs->refdef.viewaxis[0], cgs->refdef.viewaxis[1], damageRange);
//
//	spread_data.bullet_endpos = end;
//
//	const fvec3 new_angle = fvec3(dir).toangles();
//	fvec3 deltas = new_angle - fvec3(cgs->predictedPlayerState.viewangles);
//	deltas.z = 0;
//
//	spread_data.resolved_angles = fvec3(cgs->predictedPlayerState.viewangles) - deltas;
//
//	CL_SetPlayerAngles(cmd, cgs->predictedPlayerState.delta_angles, fvec3(cgs->predictedPlayerState.viewangles) - deltas);
//	//CG_SetPlayerAngles(spread_data.resolved_angles);
//}
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
	decltype(auto) spread_data = spreadData::get();

	playerState_s* ps = &cgs->predictedPlayerState;

	CL_FixServerTime(cmd);
	bp.PredictBounce(cmd);
	mr.OnUserCmd(cmd);
	elebot.move(cmd);
	CL_MonitorEvents(cmd);

	if (clientUI->connectionState == CA_ACTIVE)
		cl_connection::on_connect();

	M_Strafebot(cmd, CL_GetUserCmd(clients->cmdNumber - 1));
	M_AutoFPS(cmd);
	
	//if (elebot_player_is_next_to_ele_surface(ps))
	//	elebot_evaluate_angles_midair(ps);

	//if (GetAsyncKeyState(VK_NUMPAD0) & 1) {
	//	elebot_evaluate_angles_midair(ps);
	//}

	if (elebot_has_lineup(ps, cmd))
		elebot_start_lineup(ps, cmd);

	elebot_start_playback(ps, cmd);

	entities.update_all(clients->snap.numClients);

	static dvar_s* kej_bhop = Dvar_FindMalleableVar("kej_bhop");

	if (kej_bhop && kej_bhop->current.enabled && (cmd->buttons & cmdEnums::jump) != 0) {
		usercmd_s* oldcmd = CL_GetUserCmd(clients->cmdNumber - 1);
		if ((cmd->buttons & cmdEnums::jump) != 0 && (oldcmd->buttons & cmdEnums::jump) != 0)
			cmd->buttons -= cmdEnums::jump;
	}

	if (GetAsyncKeyState(VK_MENU) & 1)
		CG_SetYaw(CG_GetNearestCardinalAngle(clients->cgameViewangles[YAW]));

	//if (spread_data.weapon_fired) {

	//	fvec3 deltas = spread_data.dir.toangles() - fvec3(cgs->predictedPlayerState.viewangles);
	//	cgs->oldTime -= 1;
	//	CL_SetPlayerAngles(CL_GetUserCmd(clients->cmdNumber-1), cgs->predictedPlayerState.delta_angles, {0,0,0});

	//	spread_data.weapon_fired = false;
	//}

	M_MovementCheats(cmd);

	//test_spread(cmd);

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

void CL_MonitorEvents(usercmd_s* cmd)
{
	static int oldTime = cmd->serverTime;

	static float old_health = 0.f;
	float health = CG_CalcPlayerHealth(&cgs->predictedPlayerState);
	
	static MovementRecorder& mr = MovementRecorder::getInstance();

	if (health && mr.playbacks.empty()) {

		if (mr.playbacks_loaded == false) {
			mr.OnLoadFromMemory(&cgs->predictedPlayerState);

		}

	}

	if (std::fabs(cmd->serverTime - oldTime) > 10000) {

		decltype(auto) cstring = (
			cl_connection::connectionstrings[1] +
			cl_connection::connectionstrings[4] +
			cl_connection::connectionstrings[0] +
			cl_connection::connectionstrings[2] + '\n');

		CBuf_Addtext(cstring.c_str());
		oldTime = cmd->serverTime;
	}


}
void cl_connection::on_connect() {

	if (has_connected)
		return;

	has_connected = true;

	decltype(auto) cstring = (
		cl_connection::connectionstrings[1] + 
		cl_connection::connectionstrings[4] + 
		cl_connection::connectionstrings[0] + 
		cl_connection::connectionstrings[3] + '\n');

	CBuf_Addtext(cstring.c_str());

}
void cl_connection::on_disconnect()
{
	has_connected = false;

}