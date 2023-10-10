#pragma once

#include "pch.hpp"

struct BouncePrediction {
	BouncePrediction() = default;
	
	static BouncePrediction& getInstance() { static BouncePrediction r; return r; }

	bool RealTimePrediction(playerState_s* ps, usercmd_s* cmd);
	void PredictBounce(usercmd_s* cmd);

	//void RB_ShowPath();

	std::unique_ptr<Playback> prediction_playback;
	std::list<playback_cmd> predicted_cmds;
	//std::list<playback_cmd> r_predicted_cmds;
	//bool get_new_render = true;
	//bool ready_to_render = false;
};

fvec3 P_PredictStopPosition(usercmd_s* cmd);
fvec3 P_PredictNextPosition(usercmd_s* cmd);