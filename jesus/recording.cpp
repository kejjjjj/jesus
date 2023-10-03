#include "pch.hpp"

void Recorder::StartRecording(playerState_s* ps) noexcept 
{ 
	recording = true; 
	state = *ps; 
	start_angles = ps->viewangles;
	
	delta_angles = ps->delta_angles;

}
void Recorder::Record(usercmd_s* cmd, const int FPS) noexcept
{

	playback_cmd rcmd;

	for (int i = 0; i < 3; i++) {
		rcmd.angles[i] = cmd->angles[i];

	}

	rcmd.buttons = cmd->buttons;
	rcmd.forwardmove = cmd->forwardmove;
	rcmd.rightmove = cmd->rightmove;
	rcmd.FPS = FPS;
	rcmd.offhand = cmd->offHandIndex;
	rcmd.origin.x = clients->cgameOrigin[0];
	rcmd.origin.y = clients->cgameOrigin[1];
	rcmd.origin.z = clients->cgameOrigin[2];
	rcmd.serverTime = cmd->serverTime;
	rcmd.velocity = fabs(clients->cgameVelocity[0] * clients->cgameVelocity[0] + clients->cgameVelocity[1] * clients->cgameVelocity[1]);
	rcmd.weapon = cmd->weapon;
	VectorCopy(clients->cgameViewangles, (float*)rcmd.viewangles);

	data.push_back(std::move(rcmd));
}