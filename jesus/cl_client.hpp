#pragma once

#include "pch.hpp"

void CL_FinishMove(usercmd_s* cmd);
void CL_FixServerTime(usercmd_s* cmd);

void CL_WritePacket();


void CL_ParseSnapshot(msg_t* msg);