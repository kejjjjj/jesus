#pragma once

#include "pch.hpp"

inline std::unique_ptr<Playback> prediction_playback;
std::optional<std::list<playback_cmd>> RealTimePrediction(playerState_s* ps, usercmd_s* cmd);

void CL_PredictBounce(usercmd_s* cmd);