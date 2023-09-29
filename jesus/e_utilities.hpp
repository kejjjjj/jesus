#pragma once

#include "pch.hpp"

inline uintptr_t is_cod4x() {
	static const HMODULE cod4x_handle = GetModuleHandle("cod4x_021.dll");
	return reinterpret_cast<uintptr_t>(cod4x_handle);
}

template<typename Return, typename ... Args>
inline Return engine_call(const bool cod4x, const uintptr_t offset, Args... args)
{
	static const HMODULE cod4x_handle = GetModuleHandle("cod4x_021.dll");
	return (reinterpret_cast<Return(*)(Args...)>(cod4x ? reinterpret_cast<uintptr_t>(cod4x_handle) + offset : offset))(args...);
}

void CBuf_Addtext(const char* text);

cmd_function_s* Cmd_FindCommand(const char* name);
cmd_function_s* Cmd_AddCommand(const char* cmdname, void(__cdecl* function)());
cmd_function_s* Cmd_RemoveCommand(const char* cmdname);

void IN_ActivateMouse(bool active);
void IN_ActivateKeyboard(bool active);

void UI_SetSystemCursorPos(float x, float y);

void __cdecl CG_TracePoint(const vec3_t maxs, trace_t* trace, const vec3_t start, const vec3_t mins, const vec3_t end, int entityNum, int contentMask, int locational, int staticModels);

//basically just CG_TracePoint except the last 2 params are set to 0
inline void (*CG_Trace)(trace_t* result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipEntity, int tracemask) = (void(__cdecl*)(trace_t*, const vec3_t, const vec3_t, const vec3_t, const vec3_t, int, int))0x45A230;
int PM_playerTrace(pmove_t* pm, trace_t* trace, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask);
pmove_t* PM_AddTouchEnt(pmove_t* pm, int groundEntityNum);

Material* FindMaterialByName(const std::string& mtl);