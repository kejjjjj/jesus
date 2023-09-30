#include "pch.hpp"

void Playback::StartPlayback(const int serverTime, playerState_s* ps, usercmd_s* cmd, const playback_cmd& first_cmd, bool fix_deltas) noexcept
{
	if (data_original.empty())
		return;

	//Com_Printf(CON_CHANNEL_SUBTITLE, "Begin playback..\n");

	data = data_original;
	
	int subtrAmount = 0;

	Dvar_FindMalleableVar("com_maxfps")->current.integer = first_cmd.FPS;

	for (int i = 0; i < 3; i++) {
		playback_cmd precmd;
		subtrAmount += 1000 / first_cmd.FPS;

		VectorCopy(first_cmd.angles, precmd.angles);
		precmd.serverTime = first_cmd.serverTime - subtrAmount;
		precmd.FPS = first_cmd.FPS;
		precmd.forwardmove = 0;
		precmd.rightmove = 0;
		precmd.buttons = first_cmd.buttons;
		precmd.viewangles = first_cmd.viewangles;
		precmd.weapon = first_cmd.weapon;
		precmd.offhand = first_cmd.offhand;

		data.push_front(precmd);
	}

	refTime = serverTime;
	it = data.begin();
	active = true;


	if (!fix_deltas)
		return;

	int angles[3];
	int deltas[3];
	ivec3 ideltas = CL_GetPlayerAngles(cmd, cgs->snap->ps.delta_angles, first_cmd.viewangles);

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
	cmd->weapon = (it)->weapon;
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
		Com_Printf(CON_CHANNEL_OBITUARY, "end playback..\n");
		return;
	}

}
playback_cmd* Playback::CurrentCmd() const {
	if (!active)
		return 0;

	if (it == data.end() || it == data.begin())
		return nullptr;

	auto copy = it;

	return &(*copy);
}

void MovementRecorder::blabla(usercmd_s* cmd)
{
	static int angles[3] = { 0,0,0 };

	if (GetAsyncKeyState(VK_NUMPAD5) & 1) {



		ivec3 deltas = CL_GetPlayerAngles(cmd, cgs->snap->ps.delta_angles, recorder.data.front().viewangles[YAW]);

		VectorCopy(recorder.data.front().angles, angles);
		angles[YAW] = cmd->angles[YAW] + deltas.y;

		int delta = angles[YAW] - recorder.data.front().angles[YAW];

		for (auto& i : recorder.data) {
			i.angles[YAW] += delta;
		}
	}

	if ((GetAsyncKeyState(VK_NUMPAD3) < 0) == false || recorder.data.empty())
		return;

	fvec3 r_deltas = recorder.delta_angles;
	 


	cmd->angles[YAW] = angles[YAW];


}