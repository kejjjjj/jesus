#include "pch.hpp"

void MovementRecorder::OnRecorderCmd(usercmd_s* cmd)
{
	static dvar_s* com_maxfps = Dvar_FindMalleableVar("com_maxfps");

	if (GetAsyncKeyState(VK_NUMPAD4) & 1) {
		if (!recorder.isRecording()) {
			recorder.data.clear();
			Com_Printf("record!\n");
			recorder.StartRecording(&cgs->predictedPlayerState, cmd);
			playback.data_original.clear();

		}
		else {
			Com_Printf("stop!\n");
			recorder.StopRecording();
		}
	}

	if (recorder.isRecording()) {
		recorder.Record(cmd, com_maxfps->current.integer);
	}

}
void MovementRecorder::OnPlaybackCmd(usercmd_s* cmd)
{
	static bool playbackDelay = false;
	static DWORD ms = Sys_MilliSeconds();

	if (lineup_in_progress)
		return;

	if (waiting_for_playback) {

		if (!playback.isPlayback() && !playbackDelay) {
			if (playback.data_original.empty())
				playback.data_original = recorder.data;

			Com_Printf("start playback\n");
			ms = Sys_MilliSeconds();


			playback.StartPlayback(cmd->serverTime, &cgs->nextSnap->ps, cmd, recorder.data.front(), true);
			playbackDelay = true;

			CL_SetPlayerAngles(cmd, cgs->nextSnap->ps.delta_angles, recorder.data.front().viewangles);
			waiting_for_playback = false;
		}


	}

	if (playbackDelay) {

		if (Sys_MilliSeconds() > ms + 10) {
			playbackDelay = false;

			CL_SetPlayerAngles(cmd, cgs->nextSnap->ps.delta_angles, recorder.data.front().viewangles);

		}

	}
	else if (playback.isPlayback()) {
		if ((GetAsyncKeyState('W') < 0) || (GetAsyncKeyState('A') < 0) || (GetAsyncKeyState('S') < 0) || (GetAsyncKeyState('D') < 0))
			playback.active = false;
		else
			playback.doPlayback(cmd);
	}




}
void MovementRecorder::OnLineupCmd(usercmd_s* cmd)
{
	static std::unique_ptr<Lineup> lineup;

	if (recorder.isRecording())
		return;

	const auto& front = recorder.data.front();

	if (GetAsyncKeyState(VK_NUMPAD6) & 1) {
		lineup_in_progress = true;
		
		if (lineup == nullptr) {
			lineup = std::move(std::unique_ptr<Lineup>(new Lineup(front.origin, front.viewangles)));
			Com_Printf("start!\n");

		}
		else {
			lineup.reset();
			Com_Printf("stop!\n");
			lineup_in_progress = false;

		}
	}

	if (lineup_in_progress == false)
		return;




	
	if (lineup) {

		lineup->move(cmd);

		if (lineup->has_finished()) {
			lineup.reset();
			Com_Printf("angles finished!\n");
			waiting_for_playback = true;
			lineup_in_progress = false;
		}
		else if (lineup->needs_restart()) {
			lineup.reset();
			lineup = std::move(std::unique_ptr<Lineup>(new Lineup(front.origin, front.viewangles)));

		}

	}

}
void MovementRecorder::OnUserCmd(usercmd_s* cmd)
{
	OnRecorderCmd(cmd);
	OnPlaybackCmd(cmd);
	OnLineupCmd(cmd);
}

void MovementRecorder::DrawPlayback()
{
	char bufff[64];

	auto cmd = CL_GetUserCmd(clients->cmdNumber - 1);

	sprintf_s(bufff, "%.6f", std::atan2(-cmd->rightmove, cmd->forwardmove) * 180 / PI);
	R_AddCmdDrawTextWithEffects(bufff, "fonts/objectivefont", cgs->refdef.width / 1.5f - strlen(bufff) * 2, cgs->refdef.height / 1.5f - 100, 1.f, 1.f, 0, vec4_t{ 1,1,1,1 }, 3, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);


	if (recorder.isRecording()) {
		float col[4] = { 0,1,0,1 };
		float glowCol[4] = { 0,0,0,0 };
		R_AddCmdDrawTextWithEffects((char*)"recording", "fonts/normalfont", 940.f, 480.f, 1.3f, 1.3f, 0.f, col, 3, glowCol, nullptr, nullptr, 0, 0, 0, 0);
		return;
	}

	if (!playback.isPlayback())
		return;

	auto current = playback.CurrentCmd();

	if (!current)
		return;

	float dist = current->origin.dist(clients->cgameOrigin);

	char buff[64];

	sprintf_s(buff, "%.6f\ntime: %d", dist, current->serverTime);
	R_AddCmdDrawTextWithEffects(buff, "fonts/objectivefont", cgs->refdef.width / 1.5f - strlen(buff) * 2, cgs->refdef.height / 1.5f, 1.f, 1.f, 0, vec4_t{ 1,1,1,1 }, 3, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);

}