#include "pch.hpp"

void M_MovementCheats(usercmd_s* cmd)
{
	M_SuperSprint(cmd);

	//for (int i = 0; i < clients->snap.numClients; i++) {
	//	entity_s target(i);

	//	if (target.isMe())
	//		return;

	//	M_AutoKnife(cmd, &target);
	//}

}
void M_SuperSprint(usercmd_s* cmd)
{
	if (Dvar_FindMalleableVar("hack_superSprint")->current.enabled == false)
		return;

	fvec3 angles = clients->cgameViewangles;

	if (/*(cmd->buttons & cmdEnums::sprint) == 0 || */cmd->forwardmove != 127)
		return;

	if (cmd->rightmove == 0)
		cmd->rightmove = 127;

	CL_SetSilentAngles({ angles.x, angles.y, cmd->rightmove > 0 ? -90.f : 90.f });



}
void M_AutoKnife(usercmd_s* cmd, entity_s* target)
{
	if (Dvar_FindMalleableVar("hack_autoKnife")->current.enabled == false)
		return;

	entity_s self(cgs->clientNum);

	const float dist = self.getOrigin().dist(target->getOrigin());
	
	if (dist > 50)
		return;

	const float angle = (target->getOrigin() - fvec3(clients->cgameOrigin)).toangles().y;

	fvec3 angles = clients->cgameViewangles;

	CL_SetSilentAngles({ angles.x, angle, angles.z });
	if((cmd->buttons & cmdEnums::melee) == 0)
		cmd->buttons |= cmdEnums::melee;

	//Com_Printf("^2knife!\n");
}

void M_Strafebot(usercmd_s* cmd, usercmd_s* oldcmd)
{
	if (find_evar<bool>("Strafebot")->get() == false)
		return;

	T_AutoPara(&cgs->predictedPlayerState, cmd);

	std::optional<float> yaw;

	static int time_when_key_pressed = 0;
	static char most_recent_rightmove = 0;

	bool rightmove_was_pressed_this_frame = cmd->rightmove != NULL;

	//persistence
	if (rightmove_was_pressed_this_frame == false) {
		if (time_when_key_pressed + 250 > cmd->serverTime && most_recent_rightmove) {
			if (cgs->predictedPlayerState.groundEntityNum == 1023) {
				cmd->rightmove = most_recent_rightmove;
				cmd->forwardmove = 127;
			}
		}
	}

	if ((yaw = CG_GetOptYawDelta(&cgs->predictedPlayerState, cmd, oldcmd)) == std::nullopt) {
		return;
	}

	if (rightmove_was_pressed_this_frame) {
		time_when_key_pressed = cmd->serverTime;
		most_recent_rightmove = cmd->rightmove;
	}
	float delta = yaw.value();
	
	fvec3 angles = cgs->predictedPlayerState.viewangles;
	angles.y += delta;

	CL_SetPlayerAngles(cmd, cgs->predictedPlayerState.delta_angles, angles);

}
void T_AutoPara(playerState_s* ps, usercmd_s* cmd)
{
	if (find_evar<bool>("Auto Para")->get() == false || ps->speed != 190)
		return;

	const float speed = fvec2(ps->velocity[0], ps->velocity[1]).mag();
	static char _rightmove = 0;

	if ((speed <= 285 || speed >= 625) || cmd->forwardmove == NULL || ps->groundEntityNum != 1023) {
		_rightmove = 0;
		return;
	}

	if (!_rightmove)
		_rightmove = cmd->rightmove;

	if (_rightmove)
		cmd->rightmove = _rightmove;

	static std::vector<fps_zone> zones = FPS_GetZones(ps->speed);

	if (zones.empty())
		return FatalError("T_AutoPara(): no fps zones");

	const int32_t FPS = FPS_GetIdeal(ps, cmd);



	auto results = FPS_GetDistanceToZone(ps, cmd, 333);

	float distance2inneredge = _rightmove == 127 ? results.end : results.begin;

	if (distance2inneredge < 0.1f || distance2inneredge >= results.length + 10) {
		_rightmove *= -1;
	}

	cmd->rightmove = _rightmove;
}
void M_AutoFPS(usercmd_s* cmd)
{
	if (find_evar<bool>("AutoFPS")->get() == false)
		return;

	static dvar_s* com_maxfps = Dvar_FindMalleableVar("com_maxfps");

	com_maxfps->current.integer = FPS_GetIdeal(&cgs->predictedPlayerState, cmd);


}