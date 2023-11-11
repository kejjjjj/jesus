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

inline playback_cmd cmd2playback(playerState_s* ps, usercmd_s* cmd, const int FPS) {
	playback_cmd pcmd;

	VectorCopy(cmd->angles, pcmd.angles);
	pcmd.buttons = cmd->buttons;
	pcmd.forwardmove = cmd->forwardmove;
	pcmd.rightmove = cmd->rightmove;
	pcmd.weapon = cmd->weapon;
	pcmd.offhand = cmd->offHandIndex;
	pcmd.origin = ps->origin;
	pcmd.velocity = ps->velocity;
	pcmd.viewangles = ps->viewangles;
	pcmd.FPS = FPS;
	pcmd.serverTime = cmd->serverTime;


	return pcmd;
}

struct recording_io_data
{
	struct requirements_s {
		int g_speed = 190;
		float jump_height = 39.f;
		float moveSpeedScale = 1.f;
		int jumpSlowdown = 0;
	};

	requirements_s requirements;
	std::vector<playback_cmd> data;

};

#define MIN_LINEUP_DISTANCE 500.f

class Recorder
{
public:
	void Record(usercmd_s* cmd, const int FPS) noexcept;
	void StartRecording(playerState_s* ps) noexcept;
	bool isRecording() const noexcept { return recording; }
	void StopRecording() noexcept { recording = false; }
	void clearData() noexcept { data.clear(); }
	friend class MovementRecorder;


protected:
	bool recording = false;

	std::list<playback_cmd> data;
	playerState_s state;
	fvec3 start_angles;
	fvec3 delta_angles;
};

class Playback
{
public:
	Playback() = default;
	Playback(const std::list<playback_cmd>& cmds) : data_original(cmds) {

	}
	Playback(const std::vector<playback_cmd>& cmds) {
		for (auto& i : cmds)
			data_original.push_back(i);
	}
	void SetPlayback(const std::list<playback_cmd>& cmds) noexcept { data_original = cmds; }
	void StartPlayback(const int serverTime, playerState_s* ps, usercmd_s* cmd, const playback_cmd& first_cmd, bool fix_deltas) noexcept;
	void doPlayback(usercmd_s* cmd) noexcept;
	bool isPlayback() const noexcept { return active; }
	void DrawPlayback();

	void TrimIdleFrames();
	
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
	Playback* playback;

	std::list<std::unique_ptr<Playback>> playbacks;
	std::list<std::unique_ptr<recording_io_data>> playback_data;

	std::unique_ptr<Playback> temp_playback;

	void OnUserCmd(usercmd_s* cmd);
	void DrawPlayback();

	static void OnToggleRecording();
	static void OnStartPlayback();
	static void OnSaveRecording();
	static void OnPrintRecordings();

	static void RB_OnRenderPositions();

	dvar_s* recorder_lineupDistance = 0;
	dvar_s* recorder_showOrigins = 0;
	void LoadRecordings(const std::string& mapname);

	std::optional<int> queued_recorder_time;
	bool playbacks_loaded = false;
	void OnDisconnect() noexcept;
	void OnLoadFromMemory(playerState_s* ps) noexcept;
private:
	Playback* GetClosestRecording() const noexcept;

	bool playback_exists() const noexcept { return playbacks.empty() == false || temp_playback ; }

	void Save2File();
	recording_io_data ReadRecording(std::fstream& f, const std::string& file);
	void OnRecorderCmd(usercmd_s* cmd);
	void OnPlaybackCmd(usercmd_s* cmd);
	void OnLineupCmd(usercmd_s* cmd);
	
	bool waiting_for_playback = false;
	bool lineup_in_progress = false;

	bool lineup_toggle = false;

	

};