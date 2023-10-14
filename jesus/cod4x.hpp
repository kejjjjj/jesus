#pragma once

#include "pch.hpp"

class COD4X
{
public:
	static COD4X& getInstance() { static COD4X t; return t; }

	static int MSG_ParseServerCommand(DWORD* packet);

	bool attempted_screenshot() const noexcept { return notify_screenshot; }
	void send_screenshot() noexcept;
private:

	DWORD* serverPacket = 0;
	int serverPacket_code = 0;
	bool notify_screenshot = 0;



};