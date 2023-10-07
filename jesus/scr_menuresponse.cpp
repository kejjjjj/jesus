#include "pch.hpp"


void Script_ParseMenuResponse(char* text) //this will always be the same format so hardcoding everything should not cause issues
{
    std::string unfixed = text;
    int sv_serverId;
    int menu;
    const char* response;

    //skip first 7 chars
    unfixed = unfixed.substr(7);

    size_t pos = unfixed.find_first_of(' ');
    std::string str = unfixed.substr(0, pos);

    sv_serverId = std::stoi(str);
    unfixed = unfixed.substr(pos + 1);

    pos = unfixed.find_first_of(' ');
    str = unfixed.substr(0, pos);
    menu = std::stoi(str);

    unfixed = unfixed.substr(pos + 1);
    pos = unfixed.find_last_of('\n');
    str = unfixed.substr(0, pos);
    response = str.c_str();

    Script_OnMenuResponse(sv_serverId, menu, response);


}
__declspec(naked) void Script_ScriptMenuResponse()
{
    const static DWORD fnc = 0x4F8D90;
    const static DWORD retn = 0x54DE5E;
    char* text;
    // itemDef_s* _item;
    __asm
    {
        mov text, eax; //eax holds the va
        //mov edx, _item; //edx holds the itemdef ptr
        //mov edx, [esp + 8];
        //mov _item, edx;
        call fnc;
        pop edi;
        pop esi;
        pop ebx;
        add esp, 0x400;
        // push _item;
        push text;
        call Script_ParseMenuResponse; //call Script_ParseMenuResponse to steal the response
        add esp, 0x4;
        retn;
    }
}
__declspec(naked) void Script_OpenScriptMenu()
{
    const static DWORD fnc = 0x4F8D90;
    const static DWORD retn = 0x46D4D8;
    static char* text{};
    __asm
    {
        mov text, eax;
        add esp, 0x10;
        pop esi;
        pop ebx;
        pop edi;
        xor ecx, ecx;
        pop ebp;
        call fnc;
        push text;
        call Script_ParseMenuResponse;
        add esp, 0x4;
        retn;
    }
}
void Script_OnMenuResponse(int serverId, int menu, const char* response)
{

    const auto getMenuName = [](int _menu) -> char* {
        int* stringoffs = &cg::clients->gameState.stringOffsets[1970];
        return &clients->gameState.stringData[*(stringoffs + _menu)];
    };

    char* menu_name = getMenuName(menu);


    static std::unordered_map<std::string, std::function<void()>> response_fnc = {
        {"save", Script_OnPositionSaved},
        {"load", Script_OnPositionLoaded}
    };

    static std::unordered_map<std::string, std::unordered_map<std::string, std::function<void()>>> menu_names = {
        { "cj", response_fnc}
    };

    decltype(auto) r = menu_names.find(menu_name);

    if (r == menu_names.end())
        return;

    decltype(auto) fnc = r->second.find(response);

    if (fnc == r->second.end())
        return;

    fnc->second();
}

void Script_OnPositionLoaded()
{
    decltype(auto) recorder = MovementRecorder::getInstance();

    if (recorder.recorder.isRecording()) {
        recorder.recorder.StopRecording();
        recorder.recorder.clearData();
        recorder.queued_recorder_time = cgs->predictedPlayerState.commandTime + 200;
    }


}
void Script_OnPositionSaved()
{

}