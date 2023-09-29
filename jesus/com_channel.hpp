#pragma once

#pragma once

#include "pch.hpp"

void Com_Printf		 ($B87C0110D100A68234FECCEB9075A41E channel, const char* msg, ...);
void Com_Printf(const char* msg, ...);
void Com_PrintWarning($B87C0110D100A68234FECCEB9075A41E channel, const char* msg, ...);
void Com_PrintError	 ($B87C0110D100A68234FECCEB9075A41E channel, const char* msg, ...);

void iPrintLn(const char* msg);
void iPrintLnBold(const char* text, ...);