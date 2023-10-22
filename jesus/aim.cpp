#include "pch.hpp"

void Cmd_NoRecoil_f()
{
	if (cmd_args->argc[cmd_args->nesting] != 2) {
		Com_Printf(CON_CHANNEL_CONSOLEONLY, "usage: hack_norecoil <0 or 1>");
		return;
	}
	auto arg = *(cmd_args->argv[cmd_args->nesting] + 1);

	if (IsInteger(arg) == false) {
		Com_Printf(CON_CHANNEL_CONSOLEONLY, "usage: hack_norecoil <0 or 1>");
		return;
	}

	int value = std::stoi(arg);

	if (value) {
		hook::write_addr(0x44FF9D, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16); //recoil disabled
	}
	else {
		hook::write_addr(0x44FF9D, "\xD9\x5C\x24\x14\xD9\x44\x24\x14\xD9\x54\x24\x14\xD9\x1E\xD9\xC0", 16);
	}

}
