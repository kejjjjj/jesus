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

struct recording_io_data
{
	float moveSpeedScale = 1.f;
	std::vector<playback_cmd> data;

};

class Recorder
{
public:
	void Record(usercmd_s* cmd, const int FPS) noexcept;
	void StartRecording(playerState_s* ps) noexcept;
	bool isRecording() const noexcept { return recording; }
	void StopRecording() noexcept { recording = false; }

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

	std::list<std::unique_ptr<Playback>> playbacks;

	void OnUserCmd(usercmd_s* cmd);
	void DrawPlayback();

	static void OnToggleRecording();
	static void OnStartPlayback();
	static void OnSaveRecording();

	static void RB_OnRenderPositions();

	dvar_s* recorder_lineupDistance = 0;
	void LoadRecordings(const std::string& mapname);

private:
	void Save2File();
	recording_io_data ReadRecording(std::fstream& f, const std::string& file);
	void OnRecorderCmd(usercmd_s* cmd);
	void OnPlaybackCmd(usercmd_s* cmd);
	void OnLineupCmd(usercmd_s* cmd);
	
	bool waiting_for_playback = false;
	bool lineup_in_progress = false;

	bool lineup_toggle = false;

};