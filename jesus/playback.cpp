#include "pch.hpp"

void Playback::StartPlayback(const int serverTime, playerState_s* ps, usercmd_s* cmd, const playback_cmd& first_cmd, bool fix_deltas) noexcept
{
	if (data_original.empty())
		return;

	//Com_Printf(CON_CHANNEL_SUBTITLE, "Begin playback..\n");

	data = data_original;
	
	int subtrAmount = 0;

	Dvar_FindMalleableVar("com_maxfps")->current.integer = first_cmd.FPS;

	//for (int i = 0; i < 3; i++) {
	//	playback_cmd precmd;
	//	subtrAmount += 1000 / first_cmd.FPS;

	//	VectorCopy(first_cmd.angles, precmd.angles);
	//	precmd.serverTime = first_cmd.serverTime - subtrAmount;
	//	precmd.FPS = first_cmd.FPS;
	//	precmd.forwardmove = 0;
	//	precmd.rightmove = 0;
	//	precmd.buttons = first_cmd.buttons;
	//	precmd.viewangles = first_cmd.viewangles;
	//	precmd.weapon = first_cmd.weapon;
	//	precmd.offhand = first_cmd.offhand;

	//	data.push_front(precmd);
	//}

	refTime = serverTime;
	it = data.begin();
	active = true;


	if (!fix_deltas)
		return;

	int angles[3];
	int deltas[3];
	ivec3 ideltas = CL_GetPlayerAngles(cmd, ps->delta_angles, first_cmd.viewangles);

	VectorCopy(first_cmd.angles, angles);

	for (int i = 0; i < 3; i++) {

		angles[i] = cmd->angles[i] + ideltas[i];
		deltas[i] = angles[i] - first_cmd.angles[i];
	}

	for (auto& elem : data) {
		for(int i = 0; i < 3; i++)
			elem.angles[i] += deltas[i];
	}
}
void Playback::doPlayback(usercmd_s* cmd) noexcept
{
	static dvar_s* com_maxfps = Dvar_FindMalleableVar("com_maxfps");

	if (it == data.begin()) {
		refTime = cmd->serverTime;

	}

	for (int i = 0; i < 3; i++) {

		cmd->angles[i] = it->angles[i];
	}

	//CL_SetPlayerAngles(cmd, delta_angles, { SHORT2ANGLE(it->angles[0]), SHORT2ANGLE(it->angles[1]), SHORT2ANGLE(it->angles[2]) });

	cmd->serverTime = refTime + ((it)->serverTime - data.front().serverTime);
	//cmd->weapon = (it)->weapon;
	cmd->offHandIndex = (it)->offhand;
	com_maxfps->current.integer = (it)->FPS;

	clients->serverTime = cmd->serverTime;
	cmd->buttons = (it)->buttons;
	cmd->forwardmove = (it)->forwardmove;
	cmd->rightmove = (it)->rightmove;
	
	++it;

	//auto copy = it;

	//float dist = (--copy)->origin.dist(clients->cgameOrigin);

	//if (dist > 1.f) {
	//	active = false;
	//	Com_Printf(CON_CHANNEL_OBITUARY, "^1playback inconsistency!\n");
	//	return;
	//}

	if (it == data.end()) {
		active = false;
		//Com_Printf(CON_CHANNEL_OBITUARY, "end playback..\n");
		return;
	}

}
playback_cmd* Playback::CurrentCmd() const {
	if (!active)
		return 0;

	if (it == data.end())
		return nullptr;

	auto copy = it;

	return &(*copy);
}

void Playback::TrimIdleFrames()
{
	auto end = data_original.begin();

	playback_cmd front = data_original.front();

	for (auto& i : data_original) {

		if (i.forwardmove || i.rightmove || i.viewangles != front.viewangles || i.origin != front.origin || i.velocity != front.velocity)
			break;

		end++;

	}
	if(end != data_original.begin())
		data_original.erase(end);

	//Com_Printf("^3removed %i frames\n", std::distance(data_original.begin(), end));

}
void Playback::DrawPlayback()
{
	//return;
	if (!isPlayback())
		return;

	auto current = CurrentCmd();

	if (!current)
		return;

	float dist = current->origin.dist(clients->cgameOrigin);

	char buff[64];

	sprintf_s(buff, "%.6f\ntime: %d", clients->cgameOrigin[2] - current->origin.z, current->serverTime);
	R_AddCmdDrawTextWithEffects(buff, "fonts/objectivefont", cgs->refdef.width / 1.5f - strlen(buff) * 2, cgs->refdef.height / 1.5f, 1.f, 1.f, 0, vec4_t{ 1,1,1,1 }, 3, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);

}