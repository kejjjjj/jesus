#include "pch.hpp"

void CG_Init()
{
    MH_STATUS state = MH_STATUS::MH_OK;

    if (state = MH_Initialize(), state != MH_STATUS::MH_OK) {
        return FatalError(MH_StatusToString(state));
    }

    decltype(auto) renderer = Renderer::getInstance();
    decltype(auto) elebot = Elebot::getInstance();
    //decltype(auto) resources = Resources::getInstance();
    //decltype(auto) gui = Gui::getInstance();

    //hook::nop(0x04122D2); //PM_SetStrafeCondition 
    //hook::nop(0x4056DF); //BG_GetConditionBit
    //hook::write_addr(0x405360, "\xC3", 1); //BG_EvaluateConditions

    if (!renderer.initialize())
        return;

    auto cod4x = is_cod4x();

    if (is_cod4x()) {
        std::cout << "yep cod4x!\n";
        BG_WeaponNames = reinterpret_cast<WeaponDef**>(cod4x + 0x443DDE0);

    }

    Cmd_AddCommand("elebot_run", elebot.start_ground);

    //gscript.initialize();
    //resources.initialize();
    //gui.initialize();

    CG_CreatePermaHooks();

    return;
}
void CG_Cleanup()
{
    CG_ReleaseHooks();
    MH_STATUS state = MH_STATUS::MH_OK;

    if (state = MH_Uninitialize(), state != MH_STATUS::MH_OK) {
        return FatalError(MH_StatusToString(state));
    }
}