#include "pch.hpp"

void MovementRecorder::OnToggleRecording()
{
	if (NOT_SERVER)
		return;

	decltype(auto) r = getInstance();

	if (r.playback && r.playback->isPlayback())
		return Com_Printf("^1Stop the playback before recording!\n");

	if (r.recorder.isRecording()) {
		//	Com_Printf("stop!\n");
		r.temp_playback = std::move(std::unique_ptr<Playback>(new Playback(r.recorder.data)));
		r.recorder.data.clear();

		return r.recorder.StopRecording();
	}

	r.recorder.data.clear();
	//Com_Printf("record!\n");
	r.recorder.StartRecording(&cgs->predictedPlayerState);
}
void MovementRecorder::OnStartPlayback()
{
	if (NOT_SERVER)
		return;



	decltype(auto) r = getInstance();

	if (r.recorder.isRecording()) {
		return Com_Printf("^1stop recording first\n");
	}

	if (r.playback_exists() == false) {
		return Com_Printf("^1no recordings loaded\n");
	}

	r.lineup_toggle = !r.lineup_toggle;

}
void MovementRecorder::OnSaveRecording()
{
	if (NOT_SERVER)
		return;

	decltype(auto) r = getInstance();

	if (r.recorder.isRecording()) {
		return Com_Printf("^1stop recording first\n");
	}
	if (r.temp_playback == nullptr) {
		return Com_Printf("^1Can't save an empty recording\n");
	}
	if(r.playback && r.playback->isPlayback())
		return Com_Printf("^1Stop the playback before saving!\n");


	r.playback = r.temp_playback.get();

	r.Save2File();
	r.playbacks.push_back(std::move(std::unique_ptr<Playback>(new Playback(r.playback->data_original))));
	r.recorder.data.clear();
	r.temp_playback.reset();
}

void MovementRecorder::OnRecorderCmd(usercmd_s* cmd)
{
	static dvar_s* com_maxfps = Dvar_FindMalleableVar("com_maxfps");

	if (queued_recorder_time) {
		if (cmd->serverTime >= queued_recorder_time.value()) {
			
			recorder.StartRecording(&cgs->predictedPlayerState);
			queued_recorder_time = std::nullopt;
			Com_Printf("^2retry\n");
			return;
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

	if (lineup_in_progress || !playback)
		return;

	auto& first_cmd = playback->data_original.front();


	if (waiting_for_playback) {

		if (!playback->isPlayback() && !playbackDelay && playback_exists()) {
			if (playback->data_original.empty())
				playback->data_original = recorder.data;

			//Com_Printf("start playback\n");
			ms = Sys_MilliSeconds();

			playback->StartPlayback(cmd->serverTime, &cgs->nextSnap->ps, cmd, first_cmd, true);
			playbackDelay = true;

			CL_SetPlayerAngles(cmd, cgs->nextSnap->ps.delta_angles, first_cmd.viewangles);
			waiting_for_playback = false;
		}


	}

	if (playbackDelay) {

		if (Sys_MilliSeconds() > ms + 10) {
			playbackDelay = false;

			CL_SetPlayerAngles(cmd, cgs->nextSnap->ps.delta_angles, first_cmd.viewangles);

		}

	}
	else if (playback->isPlayback()) {
		if ((GetAsyncKeyState('W') < 0) || (GetAsyncKeyState('A') < 0) || (GetAsyncKeyState('S') < 0) || (GetAsyncKeyState('D') < 0))
			playback->active = false;
		else
			playback->doPlayback(cmd);

		if (playback->active == false) {

			fvec3 angles = playback->it == playback->data.end() ? fvec3(clients->cgameViewangles) : playback->it->viewangles;

			CL_SetPlayerAngles(cmd, cgs->predictedPlayerState.delta_angles, angles);
			//CG_SetYaw(playback->data.back().viewangles.y);
		}
	}




}
void MovementRecorder::OnLineupCmd(usercmd_s* cmd)
{
	static std::unique_ptr<Lineup> lineup;

	if (recorder.isRecording() || playback_exists() == false)
		return;

	static Playback* r = 0;
	static playback_cmd front;

	if (lineup_toggle) {

		r = GetClosestRecording();

		if (!r) {
			lineup_toggle = false;
			lineup_in_progress = false;
			Com_Printf("^1no nearby recordings...\n");
			return;
		}

		front = r->data_original.front();


		lineup_in_progress = true;
		
		if (lineup == nullptr) {
			lineup = std::move(std::unique_ptr<Lineup>(new Lineup(front.origin, front.viewangles, recorder_lineupDistance->current.value)));
			playback = r;
		}
		else {
			lineup.reset();
			lineup_in_progress = false;
			playback = nullptr;
		}

		lineup_toggle = false;
	}

	if (lineup_in_progress == false)
		return;

	if (lineup) {

		lineup->move(cmd);

		if (lineup->has_finished()) {
			lineup.reset();
			//Com_Printf("angles finished!\n");
			waiting_for_playback = true;
			lineup_in_progress = false;
		}
		else if (lineup->needs_restart()) {
			lineup.reset();
			lineup = std::move(std::unique_ptr<Lineup>(new Lineup(front.origin, front.viewangles, recorder_lineupDistance->current.value)));

		}
		else if (lineup->gave_up()) {
			lineup.reset();
			//		Com_Printf("^1couldn't pathfind to target\n");
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



	//sprintf_s(bufff, "%.6f", health);
	//R_AddCmdDrawTextWithEffects(bufff, "fonts/objectivefont", cgs->refdef.width / 1.5f - strlen(bufff) * 2, cgs->refdef.height / 1.5f - 100, 1.f, 1.f, 0, vec4_t{ 1,1,1,1 }, 3, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);


	if (recorder.isRecording()) {
		float col[4] = { 0,1,0,1 };
		float glowCol[4] = { 0,0,0,0 };
		R_AddCmdDrawTextWithEffects((char*)"recording", "fonts/normalfont", 940.f, 480.f, 1.3f, 1.3f, 0.f, col, 3, glowCol, nullptr, nullptr, 0, 0, 0, 0);
		return;
	}

	return;

	if (!playback || !playback->isPlayback())
		return;

	auto current = playback->CurrentCmd();

	if (!current)
		return;

	float dist = current->origin.dist(clients->cgameOrigin);

	char buff[64];

	sprintf_s(buff, "%.6f\ntime: %d", dist, current->serverTime);
	R_AddCmdDrawTextWithEffects(buff, "fonts/objectivefont", cgs->refdef.width / 1.5f - strlen(buff) * 2, cgs->refdef.height / 1.5f, 1.f, 1.f, 0, vec4_t{ 1,1,1,1 }, 3, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);

}

void MovementRecorder::RB_OnRenderPositions()
{
	decltype(auto) r = getInstance();

	std::uint8_t c[4];

	const Pixel col = generateRainbowColor();

	((char(__fastcall*)(const float* in, uint8_t * out))0x493530)(vec4_t{col.r/255.f,col.g / 255.f,col.b / 255.f,0.3f}, c);


	for (auto& i : r.playbacks) {

		const fvec3& origin = i->data_original.front().origin;
		auto pts = Geom_CreatePyramid(origin, {7,7, 14}, fmodf((Sys_MilliSeconds() / 60.f), 360.f));

		RB_DrawPolyInteriors(pts.size(), pts, c, true, true);

	}


}
Playback* MovementRecorder::GetClosestRecording() const noexcept
{
	if (temp_playback)
		return temp_playback.get();

	float fclosest = MIN_LINEUP_DISTANCE;
	fvec3 org;
	float dist = 0.f;
	Playback* closest = 0;
	for (auto& i : playbacks) {
		org = (i->data_original.front().origin);

		if ((dist = org.dist(clients->cgameOrigin)) < fclosest) {
			fclosest = dist;
			closest = i.get();
		}

	}

	return closest;
}

void MovementRecorder::OnDisconnect() noexcept
{
	recorder.StopRecording();
	recorder.clearData();

	playbacks.clear();
	playback = 0;
	temp_playback.reset();
	queued_recorder_time = std::nullopt;

	waiting_for_playback = false;
	lineup_in_progress = false;

	lineup_toggle = false;
}
void MovementRecorder::OnRespawn(playerState_s* ps) noexcept
{
	const float moveSpeedScaleMultiplier = ps->moveSpeedScaleMultiplier;
	const int speed = ps->speed;
	playbacks.clear();

	for (auto& i : playback_data) {

		recording_io_data::requirements_s& r = i->requirements;

		if(r.g_speed == speed && r.moveSpeedScale == moveSpeedScaleMultiplier)
			playbacks.push_back(std::move(std::unique_ptr<Playback>(new Playback(i->data))));

	}


}