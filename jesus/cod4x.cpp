#include "pch.hpp"

int COD4X::MSG_ParseServerCommand(DWORD* packet)
{
	static decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_COD4X_SCREENSHOT);
    static decltype(auto) x = getInstance();

    if(NOT_SERVER)
        return detour_func.cast_call<int(*)(DWORD*)>(packet);


	int code = ((int(*)(DWORD * packet))(is_cod4x() + 0x58489))(packet);
	std::cout << "MSG_ParseServerCommand() called with packet: 0x" << std::hex << code << " | from: 0x" << std::hex << packet << '\n';
	std::cout << "code as dec: " << std::dec << code << '\n';


    if (code == 0x753 || code == 0x666 || code == 0x866) {

        x.serverPacket = packet;
        x.serverPacket_code = code;
        x.notify_screenshot = true;

        std::cout << "Attempted screenshot!\n";
        *(DWORD*)(packet + 0x1C) = *(DWORD*)(packet + 0x14);
        return 0;

    }

	return detour_func.cast_call<int(*)(DWORD*)>(packet);
}
void COD4X::send_screenshot() noexcept
{
    std::cout << "sending clean screenshot...";
    ((void(*)(DWORD * serverpacket, DWORD packet))(is_cod4x() + 0xEA610))(serverPacket, serverPacket_code); 
    serverPacket = 0;
    serverPacket_code = 0;
    notify_screenshot = false;

    std::cout << "  done!\n";

}