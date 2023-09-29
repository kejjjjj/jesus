#pragma once

#include "pch.hpp"

struct playback_cmd
{
	int serverTime = 0;
	int angles[3];
	int32_t buttons = 0;
	char forwardmove = 0;
	char rightmove = 0;
	int FPS = 0;
	char weapon = 0;
	char offhand = 0;
	fvec3 viewangles;
	fvec3 origin;
	fvec3 velocity;
	fvec3 mins, maxs;
	float camera_yaw;
};


class Recorder
{
public:
	void Record(usercmd_s* cmd, const int FPS) noexcept;
	void StartRecording(playerState_s* ps, usercmd_s* cmd) noexcept;
	bool isRecording() const noexcept { return recording; }
	void StopRecording() noexcept { recording = false; }

	friend class MovementRecorder;


protected:
	bool recording = false;

	std::list<playback_cmd> data;
	playerState_s state;
	usercmd_s state_cmd;
	fvec3 start_angles;
	fvec3 delta_angles;
	usercmd_s oldcmd;
};

class Playback
{
public:
	Playback() = default;
	Playback(const std::list<playback_cmd>& cmds) : data_original(cmds) {

	}
	void SetPlayback(const std::list<playback_cmd>& cmds) noexcept { data_original = cmds; }
	void StartPlayback(const int serverTime, playerState_s* ps, usercmd_s* cmd, const playback_cmd& first_cmd, bool fix_deltas) noexcept;
	void doPlayback(usercmd_s* cmd) noexcept;
	bool isPlayback() const noexcept { return active; }
	
	playback_cmd* CurrentCmd() const;


	friend class MovementRecorder;

private:
	int refTime = 0;
	std::list<playback_cmd> data;
	std::list<playback_cmd> data_original;

	std::list<playback_cmd>::iterator it;
	bool active = false;
	fvec3 delta_angles;
};

class MovementRecorder : public Recorder, public Playback
{
public:
	static MovementRecorder& getInstance() { static MovementRecorder r; return r; }

	Recorder recorder;
	Playback playback;

private:
	void OnRecorderCmd(usercmd_s*cmd)
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
	void OnPlaybackCmd(usercmd_s* cmd)
	{
		static bool playbackDelay = false;
		static DWORD ms = Sys_MilliSeconds();

		if (GetAsyncKeyState(VK_NUMPAD6) & 1) {

			if (!playback.isPlayback() && !playbackDelay) {
				if(playback.data_original.empty())
					playback.data_original = recorder.data;

				Com_Printf("start playback\n");
				ms = Sys_MilliSeconds();


				playback.StartPlayback(cmd->serverTime, &cgs->nextSnap->ps, cmd, recorder.data.front(), true);
				playbackDelay = true;
				//*ps_loc = recorder.state;

				//VectorCopy(recorder.state.delta_angles, ps_loc->delta_angles);
				//VectorCopy(recorder.state.viewangles, ps_loc->viewangles);

				CL_SetPlayerAngles(cmd, cgs->nextSnap->ps.delta_angles, recorder.delta_angles);

			}


		}

		if (playbackDelay) {

			if (Sys_MilliSeconds() > ms + 1000) {
				playbackDelay = false;

				CL_SetPlayerAngles(cmd, cgs->nextSnap->ps.delta_angles, recorder.start_angles);

			}

		}
		else if (playback.isPlayback()) {
			if ((GetAsyncKeyState('W') < 0) || (GetAsyncKeyState('A') < 0) || (GetAsyncKeyState('S') < 0) || (GetAsyncKeyState('D') < 0))
				playback.active = false;
			else
				playback.doPlayback(cmd);
		}

		if (GetAsyncKeyState(VK_NUMPAD9) < 0 ) {
			//for(int i =0 ; i < 3; i++)
			//	cmd->angles[i] += ANGLE2SHORT(clients->cgameViewangles[i]);

			

			//ps_to->delta_angles[YAW] = 0;
	//		cmd->serverTime = cgs->oldTime + 1;

			//for (int i = 0; i < 32; i++) {
			//	if (&clients->snapshots[i]) {
			//		clients->snapshots[i].ps.delta_angles[YAW] = 0;
 		//		}
			//}

			//cgs->activeSnapshots[0].ps.delta_angles[YAW] = 0;
			//cgs->activeSnapshots[1].ps.delta_angles[YAW] = 0;

			//CG_SetDeltaAngles(vec3_t{ 0,90,0 });

			//std::cout << std::hex << &clients->snapshots << '\n';
			//
			////CL_SetPlayerAngles(cmd, clients->snap.ps.delta_angles, { 0,90,0 });
			//VectorClear(ps_loc->delta_angles);
			static int i = 0;

			if ((i++) >= 65535)
				i = 0;

			cmd->angles[YAW] = i;

		}



	}
public:
	void blabla(usercmd_s* cmd);
	void OnUserCmd(usercmd_s* cmd)
	{
		OnRecorderCmd(cmd);
		OnPlaybackCmd(cmd);

	}

	void DrawPlayback()
	{
		char bufff[64];

		sprintf_s(bufff, "%.6f", cgs->snap->ps.delta_angles[YAW]);
		R_AddCmdDrawTextWithEffects(bufff, "fonts/objectivefont", cgs->refdef.width / 1.5f - strlen(bufff) * 2, cgs->refdef.height / 1.5f-100, 1.f, 1.f, 0, vec4_t{ 1,1,1,1 }, 3, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);


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
	

};