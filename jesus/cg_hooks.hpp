#pragma once

#include "pch.hpp"

enum class hookEnums_e : short
{
	//PERMA HOOKS
	HOOK_SHUTDOWN_RENDERER,
	HOOK_RECOVER_LOST_DEVICE,
	HOOK_WNDPROC,

	HOOK_SV_MAP,
	HOOK_CL_DISCONNECT,


	//TEMP HOOKS
	HOOK_ENDSCENE,
	HOOK_DRAWACTIVE,
	
	HOOK_GSCR_GETFUNCTION,
	HOOK_GSCR_GETMETHOD,

	HOOK_PM_WALKMOVE,
	HOOK_PM_AIRMOVE,
	HOOK_PM_UPDATE_VIEWANGLES,

	HOOK_RB_ENDSCENE,

	HOOK_CL_MOUSEMOVE,
	HOOK_CL_FINISHMOVE,

	HOOK_CL_WRITEPACKET,

	HOOK_SCR_SCRIPTMENURESPONSE,
	HOOK_SCR_OPENSCRIPTMENU,

	HOOK_CL_PARSESNAPSHOT,
	HOOK_LOADMAP_LOADSCREEN,

	HOOK_CMD_EXECUTESINGLE,
	HOOK_GET_DVAR_INT,
	HOOK_ENDSCENE_FIX,

	HOOK_COD4X_SCREENSHOT
};

class HookTable
{
public:
	static HookTable& getInstance() {
		static HookTable _table;
		return _table;
	}
	decltype(auto) find(const hookEnums_e e) const {
		auto found = table.find(e);
		return found != table.end() ? std::make_optional(found) : std::nullopt;
	}
	decltype(auto) [[maybe_unused]] insert(const hookEnums_e e, const hook::hookobj<void*>& h) {

		return &table.insert({ e, h }).first->second;
	}
	void enable(const hookEnums_e h) {
		auto found = table.find(h);

		if (found == table.end())
			return;

		found->second.enable();
	}
	void disable(const hookEnums_e h) {
		auto found = table.find(h);

		if (found == table.end())
			return;

		found->second.disable();
	}
	decltype(auto) getter() const noexcept { return std::ref(table); };
private:
	std::unordered_map<hookEnums_e, hook::hookobj<void*>> table;
};


hook::hookobj<void*>& find_hook(const hookEnums_e hook);


void CG_CreatePermaHooks();
void CG_CreateHooks();
void CG_ReleaseHooks();
