#include "pch.hpp"

void CG_Init()
{
    MH_STATUS state = MH_STATUS::MH_OK;

    if (state = MH_Initialize(), state != MH_STATUS::MH_OK) {
        return FatalError(MH_StatusToString(state));
    }

    decltype(auto) renderer = Renderer::getInstance();
    decltype(auto) elebot = Elebot::getInstance();
    decltype(auto) recorder = MovementRecorder::getInstance();
    //decltype(auto) resources = Resources::getInstance();
    //decltype(auto) gui = Gui::getInstance();

    hook::nop(0x04122D2); //PM_SetStrafeCondition 
    hook::nop(0x4056DF); //BG_GetConditionBit
    hook::write_addr(0x405360, "\xC3", 1); //BG_EvaluateConditions

   // hook::write_addr(0x41AA40, "\xC3", 1); //BG_GetVerticalBobFactor
   // hook::write_addr(0x41AB00, "\xC3", 1); //BG_GetHorizontalBobFactor


    if (!renderer.initialize())
        return;

    auto cod4x = is_cod4x();

    if (is_cod4x()) {
        std::cout << "yep cod4x!\n";
        BG_WeaponNames = reinterpret_cast<WeaponDef**>(cod4x + 0x443DDE0);

        hook::write_addr((cod4x + 0x43580), "\xC3", 1); //put a return to the beginning of the function that checks for hooks and crashes the game if a hook is installed.. nice cod4x
        hook::write_addr(0x434200, "\x51\x53\x8B\x5C\x24", 5);  //remove the CG_DObjGetLocalTagMatrix hook from cod4x

        hook::write_addr((cod4x + 0x431B0), "\xC3", 1); //put a return to the beginning of CG_DObjGetLocalTagMatrix

    }

    CG_CreateSubdirectory("");
    CG_CreateSubdirectory("recorder");


    //Cmd_AddCommand("elebot_run", elebot.start_ground);
    Cmd_AddCommand("recorder_record", recorder.OnToggleRecording);
    Cmd_AddCommand("recorder_save", recorder.OnSaveRecording);
    Cmd_AddCommand("recorder_playback", recorder.OnStartPlayback);
    Cmd_AddCommand("recorder_printAll", recorder.OnPrintRecordings);
    Cmd_AddCommand("recorder_reloadPlaybacks", []() { return MovementRecorder::getInstance().OnLoadFromMemory(&cgs->predictedPlayerState); });


    recorder.recorder_lineupDistance = Dvar_RegisterFloat("recorder_lineupDistance", 0.01f, 0.f, 1.f, dvar_flags::saved,
        "how close to the origin of the playback the bot will attempt to move to; lower value -> better playback");

    recorder.recorder_showOrigins = Dvar_RegisterBool("recorder_showOrigins", dvar_flags::saved, true, "draws a cone at the lineup positions");

    Dvar_RegisterBool("kej_bhop", dvar_flags::saved, false, "bhop when holding spacebar");
    Dvar_RegisterBool("kej_easyBounces", dvar_flags::saved, false, "makes bouncing a lot easier");

    Dvar_RegisterBool("hack_superSprint", dvar_flags::saved, false, "run a lot faster yea");
    Dvar_RegisterBool("hack_autoKnife", dvar_flags::saved, false, "knife everyone near you automatically");

    Dvar_RegisterBool("hack_playerNames", dvar_flags::saved, false, "render player names");
    Dvar_RegisterBool("hack_playerWeapons", dvar_flags::saved, false, "render player weapons");
    Dvar_RegisterBool("hack_circularCompass", dvar_flags::saved, false, "a better compass");


    hook::write_addr(0x6496DB, "\xEB\x00\xBA\xF0\xF5", 5); //jnz -> jmp

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