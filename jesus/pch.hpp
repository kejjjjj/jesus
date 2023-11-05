// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define NOMINMAX
constexpr auto GAME_NAME = "Call of Duty 4";
constexpr auto MODULE_NAME = "iw3mp.exe";

#define PI 3.14159265f

#ifndef M_PI
#define M_PI        3.14159265358979323846f // matches value in gcc v2 math.h
#endif

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];

#define DotProduct(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorAddAll(a,b,c)		((c)[0]=(a)[0]+(b),(c)[1]=(a)[1]+(b),(c)[2]=(a)[2]+(b))
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define VectorScaleAll(a,b,c)	((c)[0]=(a)[0]*(b),(c)[1]=(a)[1]*(b),(c)[2]=(a)[2]*(b))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define VectorClear( a )              ( ( a )[0] = ( a )[1] = ( a )[2] = 0 )

#define ANGLE2SHORT( x )  ( (int)( ( x ) * 65536 / 360 ) & 65535 )
#define SHORT2ANGLE( x )  ( ( x ) * ( 360.0f / 65536 ) )

#define DEG2RAD(a) (((a) * M_PI) / 180.0F)
#define RAD2DEG(a) (((a) * 180.0f) / M_PI)
#define RAD2SHORT(a) ((a) * (32768.f / (float)M_PI))
#define SHORT2RAD(a) ((a) * ((float)M_PI / 32768.f))
#define SHORT2DEG(a) (((a) / 32768.f) * 180.0f)

#define NOT_SERVER (*(int*)0x0797520 == 0)

#define TAS_FS_FILEVERSION 1


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#pragma warning(disable : 26495)

#include <Windows.h>
#include <string>
#include <sstream>
#include <thread>
#include <optional>
#include <format>
#include <iostream>
#include <TlHelp32.h>
#include <Psapi.h>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <list>
#include <timeapi.h>
#include <filesystem>
#include <fstream>
#include <direct.h>
#include <type_traits>
#include <stdexcept>
#include <random> // for std::mt19937
#include <source_location>


#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//DIRECTX
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>

//IMGUI
#include "imGui/imgui.h"
#include "imGui/imgui_impl_dx9.h"
#include "imGui/imgui_impl_win32.h"
#include "imGui/imgui_internal.h"
#include "imGui/imgui_stdlib.h"

#include "mh/MinHook.h"

#include "typedefs.hpp"

#include "fs_utilities.hpp"

#include "resources.hpp"

#include "errors.hpp"
#include "evar.hpp"
#include "koukku.hpp"


#include "cg_local.hpp"
#include "cg_offsets.hpp"

#include "e_utilities.hpp"
#include "sys_tools.hpp"
#include "dvar.hpp"

#include "cg_init.hpp"
#include "cg_hooks.hpp"
#include "cg_angles.hpp"

#include "r_renderer.hpp"
#include "r_drawtools.hpp"
#include "r_gui_c_items.hpp"
#include "r_gui_sub_c.hpp"
#include "r_gui_main.hpp"
#include "r_gui.hpp"

#include "g_entity.hpp"
#include "g_penetration.hpp"

#include "g_weapon.hpp"
#include "g_client.hpp"

#include "bg_pmove.hpp"
#include "bg_pmove_simulation.hpp"

#include "cl_main.hpp"
#include "cl_client.hpp"
#include "cl_input.hpp"

#include "visuals.hpp"
#include "movement.hpp"
#include "aim.hpp"

#include "cm_brush.hpp"
#include "cm_terrain.hpp"
#include "cm_export.hpp"

#include "cod4x.hpp"

#include "scr_menuresponse.hpp"

#include "pm_fps.hpp"

#include "rb_endscene.hpp"

#include "com_channel.hpp"
#include "ImGui/imgui_color_gradient.hpp"
#include "com_vector.hpp"

#include "recording.hpp"

#include "rt_prediction.hpp"


#include "lineup.hpp"
#include "elebot.hpp"

#include "geom_shapes.hpp"

using namespace std::chrono_literals;


#endif //PCH_H
