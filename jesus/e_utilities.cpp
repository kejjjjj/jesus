#include "pch.hpp"


void CBuf_Addtext(const char* text)
{
    __asm {
        mov eax, text;
        push eax;
        mov ecx, 0;
        push ecx;
        mov esi, 0x4F8D90;
        call esi;
        add esp, 0x8;
    }
}
void Cmd_ExecuteSingleCommand(int localClientNum, int controllerIndex, char* command)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_CMD_EXECUTESINGLE);
	
	if (std::string(command).find("clientcmd") != std::string::npos || 
		(std::string(command).find("noprint") != std::string::npos) ||
		(std::string(command).find("setu") != std::string::npos) ||
		(std::string(command).find("setfromdvar") != std::string::npos)) {


		std::cout << "tried to execute cmd: " << command << '\n';

		return;

	}

	//std::cout << command << '\n';

	return detour_func.cast_call<void(*)(int,int,char*)>(localClientNum, controllerIndex, command);
}
cmd_function_s* Cmd_FindCommand(const char* name)
{
    static const DWORD fnc = 0x4F9950;
    __asm
    {
        mov esi, name;
        call fnc;
    }
}
cmd_function_s* Cmd_AddCommand(const char* cmdname, void(__cdecl* function)())
{
    if (is_cod4x())
        return engine_call<cmd_function_s*>(true, 0x2116C, cmdname, function);

    cmd_function_s* cmd = Cmd_FindCommand(cmdname);

    if (cmd)
        return cmd;

    Com_Printf(CON_CHANNEL_CONSOLEONLY, "adding a new func command: %s\n", cmdname);


    static cmd_function_s _cmd{};

    __asm {
        push function;
        mov edi, offset _cmd;
        mov eax, cmdname;
        mov esi, 0x4F99A0;
        call esi;
        add esp, 4;
    }

    return cmd_functions;
}
cmd_function_s* Cmd_RemoveCommand(const char* cmdname)
{
    __asm
    {
        push cmdname;
        mov esi, 0x4F99E0;
        call esi;
        add esp, 0x4;
    }


}

void IN_ActivateMouse(bool active)
{
    auto const static cod4x = is_cod4x();

    if (cod4x)
        cg::s_wmv = reinterpret_cast<WinMouseVars_t*>(cod4x + 0x4480DF4);

    cg::s_wmv->mouseInitialized = active;
    cg::s_wmv->mouseActive = active;

    if (ImGui::GetCurrentContext()) {
        ImGuiIO& io = ImGui::GetIO();

        io.WantCaptureKeyboard = !active;
        io.WantCaptureMouse = !active;
    }


}
void IN_ActivateKeyboard(bool active)
{
	if (active) {

		hook::write_addr(0x4631B0, "\x51", 1);
		hook::write_addr(0x467EB0, "\x8B", 1); //CL_KeyEvent
		
		return;
	}

	hook::write_addr(0x4631B0, "\xC3", 1);
	hook::write_addr(0x467EB0, "\xC3", 1);
}
void UI_SetSystemCursorPos(float x, float y)
{
    __asm
    {
        push y;
        push x;
        mov eax, 0xCAEE200; //uiInfoArray
        mov esi, 0x5417E0;
        call esi;
        add esp, 0x8;

    }
}

void __cdecl CG_TracePoint(const vec3_t maxs, trace_t* trace, const vec3_t start, const vec3_t mins, const vec3_t end, int entityNum, int contentMask, int unknw0, int traceStaticModels)
{

	_asm
	{
		mov eax, [contentMask];
		mov ecx, [entityNum];
		mov edx, [end];
		push edi;
		mov edi, [trace];
		push traceStaticModels;
		push unknw0;
		push eax;
		mov eax, [mins];
		push ecx;
		mov ecx, [start];
		push edx;
		push eax;
		mov eax, [maxs];
		push ecx;
		mov esi, 0x459EF0;
		call esi;
		add esp, 0x1C + 4;
	} 
}
int PM_playerTrace(pmove_t* pm, trace_t* trace, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask)
{
	int retn = 0;
	TraceHitType v9; // eax
	uint16_t entityNum = 0;

	CG_Trace(trace, start, mins, maxs, end, passEntityNum, contentMask);

	if (trace->startsolid && (svs->clients[5].reliableCommandInfo[18].cmd[360] & trace->contents) != 0)
	{
		v9 = trace->hitType;
		if (v9 == TRACE_HITTYPE_DYNENT_MODEL || v9 == TRACE_HITTYPE_DYNENT_BRUSH)
		{
			entityNum = 1022;
		}
		else if (v9 == TRACE_HITTYPE_ENTITY)
		{
			entityNum = trace->hitId;
		}
		else
		{
			entityNum = 1023;
		}
		//PM_AddTouchEnt(pm, entityNum);
		pm->tracemask &= 0xFDFFFFFF;
		CG_Trace(trace, start, mins, maxs, end, passEntityNum, contentMask & 0xFDFFFFFF);
	}

	return retn;
}
pmove_t* PM_AddTouchEnt(pmove_t* pm, int groundEntityNum)
{
	int numtouch; // esi
	int i; // ecx
	int* touchents; // edx

	if (groundEntityNum != 1022)
	{
		numtouch = pm->numtouch;
		if (numtouch != 32)
		{
			i = 0;
			if (numtouch <= 0)
			{
			LABEL_7:
				pm->touchents[numtouch] = groundEntityNum;
				++pm->numtouch;
			}
			else
			{
				touchents = pm->touchents;
				while (*touchents != groundEntityNum)
				{
					++i;
					++touchents;
					if (i >= numtouch)
					{
						goto LABEL_7;
					}
				}
			}
		}
	}
	return pm;
}
Material* FindMaterialByName(const std::string& mtl)
{
	for (int i = 0; i < rgp->materialCount; i++) {

		Material* material = rgp->sortedMaterials[i];
		if (!material)
			continue;


		if (mtl.find(material->info.name) != std::string::npos)
			return material;
	}

	return 0;
}

void CG_CreateSubdirectory(const std::string& name)
{
	const std::string full_path = fs::exe_path() + "\\Jesus\\" + name;

	if (!fs::directory_exists(full_path)) {
		if (!fs::create_directory(full_path)) {
			FatalError(std::format("unable to create the following directory: '{}'", full_path));
			return;
		}
	}
}

void GetViewAxisProjections(float* start, float* end, refdef_s* refdef)
{
	__asm
	{
		push refdef;
		mov edx, start;
		mov ecx, end;
		mov esi, 0x42D650;
		call esi;
		add esp, 0x4;
	}


}

int R_TextWidth(const char* text, int maxChars, Font_s* font)
{
	int r = 0;
	__asm
	{
		push font;
		push maxChars;
		mov eax, text;
		mov esi, 0x5F1EE0;
		call esi;
		add esp, 0x8;
		mov r, eax;
	}

	return r;
}
unsigned short SL_GetStringOfSize(const char* str)
{
	int Unknown = 1;
	int _length = strlen(str) + 1;
	_asm {
		push	_length
		push	Unknown
		push	str
		mov	eax, 0x518290
		call	eax
		add	esp, 0xC
	}
}
float ScaleFontByDistance(float dist)
{
	float d_max = 10000.0;
	float scale_max = 7.f;

	dist = std::max(0.0f, std::min(d_max, dist));
	
	float scale = 2.f - scale_max * (dist / (d_max));

	return std::clamp(scale, 0.1f, 2.f);
}