#pragma once

#include "pch.hpp"

void M_MovementCheats(usercmd_s* cmd);
void M_SuperSprint(usercmd_s* cmd);
void M_AutoKnife(usercmd_s* cmd, entity_s* target);

void M_Strafebot(usercmd_s* cmd, usercmd_s* oldcmd);
void T_AutoPara(playerState_s* ps, usercmd_s* cmd);
void M_AutoFPS(usercmd_s* cmd);