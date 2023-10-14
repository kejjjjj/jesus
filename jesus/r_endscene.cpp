#include "pch.hpp"

std::vector<fvec3> points;
fvec3 ref_org;


long __stdcall Renderer::EndSceneRenderer(IDirect3DDevice9* device)
{
	static Renderer& renderer = Renderer::getInstance();
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_ENDSCENE);
	static BouncePrediction& bp = BouncePrediction::getInstance();
	static decltype(auto) cod4x = COD4X::getInstance();

	static bool canSS = false;
	
	if (cod4x.attempted_screenshot()) {
		canSS = true; //clear the screen with one clean frame

		//tähän kohtaa renderaa kuva neekerin kyrvästä

		return detour_func.cast_call<long(__stdcall*)(IDirect3DDevice9*)>(device);

	}
	
	if (canSS) {
		//now that the most recent frame was clean, send it to the server
		cod4x.send_screenshot();
		canSS = false;
	}

	if (!ImGui::GetCurrentContext()) {
		renderer.initialize();
		std::cout << "create imgui\n";
		return detour_func.cast_call<long(__stdcall*)(IDirect3DDevice9*)>(device);

	}

	if (renderer.begin_frame()) {

		//bp.RB_ShowPath();


		renderer.end_frame();
	}

	return detour_func.cast_call<long(__stdcall*)(IDirect3DDevice9*)>(device);
}


void __cdecl Renderer::CG_DrawActive()
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_DRAWACTIVE);
	static MovementRecorder& mr = MovementRecorder::getInstance();

	if (COD4X::getInstance().attempted_screenshot())
		return detour_func.cast_call<void(__cdecl*)()>();

	for (int i = 0; i < clients->snap.numClients; i++) {
		entity_s target(i);
		R_OwnerDraw(&target);

		if(target.isEnemy() && target.isValid() && !target.isMe())
			M_AutoKnife(CL_GetUserCmd(clients->cmdNumber), &target);
	}

	char buffer[128];

	sprintf_s(buffer, "x:     %.6f\ny:     %.6f\nz:     %.6f\nyaw: %.6f\nspeed: %.6f", clients->cgameOrigin[0], clients->cgameOrigin[1], clients->cgameOrigin[2], (clients->cgameViewangles[YAW]), fvec2(clients->cgameVelocity).mag());

	float col[4] = { 0,1,0,1 };
	float glowCol[4] = { 1,0,0,1 };


	R_AddCmdDrawTextWithEffects(buffer, "fonts/normalfont", 10.f, 480.f, 1.3f, 1.3f, 0.f, vec4_t{ 0,1,0,1 }, 3, vec4_t{0,0,0,0}, nullptr, nullptr, 0, 0, 0, 0);

	mr.DrawPlayback();

	//if (prediction_playback)
	//	prediction_playback->DrawPlayback();

	return detour_func.cast_call<void (__cdecl*)()>();
}