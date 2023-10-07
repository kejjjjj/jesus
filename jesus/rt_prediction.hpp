#pragma once

#include "pch.hpp"

std::optional<std::list<playback_cmd>> RealTimePrediction(playerState_s* ps, usercmd_s* cmd);