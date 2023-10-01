#pragma once

#include "pch.hpp"

inline usercmd_s* CL_GetUserCmd(const int cmdNumber) {
	return &clients->cmds[cmdNumber & 0x7F];
}

void CL_FinishMove(usercmd_s* cmd);
void CL_FixServerTime(usercmd_s* cmd);

void CL_WritePacket();


void CL_ParseSnapshot(msg_t* msg);