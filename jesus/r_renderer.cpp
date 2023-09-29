#include "pch.hpp"

bool Renderer::initialize()
{
	std::this_thread::sleep_for(500ms);
	h_hWnd = FindWindow(NULL, is_cod4x() ? "Call of Duty 4 X" : "Call of Duty 4");

	if (IsWindow(h_hWnd) == false) {
		FatalError("failed to get game window handle");
		return false;
	}

	decltype(get_endscene()) endscene;
	//decltype(auto) resources = Resources::getInstance();
	if ((endscene = get_endscene()).has_value() == false) {
		FatalError("failed to obtain endscene pointer");
		return false;
	}

	pEndScene = endscene.value();

	pDevice = dx->device;

	initialize_imgui();

	//resources.recreate_materials();
	
	std::cout << "endscene pointer: " << pEndScene << '\n';

	return true;

}
std::optional<endScene> Renderer::get_endscene()
{
	PVOID* vTable = *reinterpret_cast<PVOID**>(cg::dx->device);
	pEndScene = reinterpret_cast<endScene>(vTable[42]);
	return pEndScene;
}
void Renderer::initialize_imgui()
{
	if (!pDevice) {
		FatalError("!pDevice");
		return;
	}

	ImGui::CreateContext();
	//ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	//std::string font_dir = "C:\\Windows\\Fonts\\Arial.ttf";
	//io.Fonts->AddFontFromFileTTF(font_dir.c_str(), 12 * 2);
	//io.Fonts->Build();

	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(h_hWnd);
	ImGui_ImplDX9_Init(pDevice);

	io.WantCaptureMouse = true;
	io.WantCaptureKeyboard = true;

	create_theme(1.f);
}

bool Renderer::begin_frame()
{
	if (!ImGui::GetCurrentContext())
		return false;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return true;
}
void Renderer::end_frame()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
void Renderer::create_theme(const float transparency)
{
	const ImVec4 widget_color = ImVec4(127.f / 255, 21.f / 255, 60.f / 255, transparency);
	const ImVec4 main_color = ImVec4(30.f / 255, 30.f / 255, 41.f / 255, transparency);
	const ImVec4 window_color = ImVec4(33.f / 255, 34.f / 255, 51.f / 255, transparency);
	const ImVec4 widget_active = ImVec4(40.f / 255, 40.f / 255, 40.f / 255, transparency);
	const ImVec4 separator_color = ImVec4(117.f / 255, 22.f / 255, 46. / 255, transparency);

	auto& style = ImGui::GetStyle();
	auto& color = style.Colors;
	style.FrameRounding = 10.f;
	style.WindowRounding = 10.f;
	style.ChildRounding = 10.f;
	style.TabRounding = 10.f;
	style.GrabRounding = 10.f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.FrameBorderSize = 1.;
	/*style.WindowPadding = ImVec2(0, 0);*/
	color[ImGuiCol_CheckMark] = widget_color;
	color[ImGuiCol_SliderGrabActive] = widget_color;
	color[ImGuiCol_SliderGrab] = widget_color;
	//color[ImGuiCol_PopupBg] = widget_color;

	color[ImGuiCol_TitleBg] = main_color;
	color[ImGuiCol_TitleBgActive] = main_color;
	color[ImGuiCol_FrameBgActive] = main_color;
	color[ImGuiCol_FrameBg] = main_color;
	color[ImGuiCol_FrameBgHovered] = main_color;

	color[ImGuiCol_WindowBg] = window_color;

	color[ImGuiCol_ResizeGrip] = main_color;
	color[ImGuiCol_ResizeGripActive] = main_color;
	color[ImGuiCol_Button] = widget_active;
	color[ImGuiCol_ButtonActive] = widget_active;
	color[ImGuiCol_ButtonHovered] = widget_active;

	color[ImGuiCol_HeaderHovered] = widget_color;
	color[ImGuiCol_HeaderActive] = widget_color;
	color[ImGuiCol_Header] = widget_color;

	color[ImGuiCol_Separator] = separator_color;
	color[ImGuiCol_SeparatorActive] = separator_color;
	color[ImGuiCol_SeparatorHovered] = separator_color;

}
void Renderer::CL_ShutdownRenderer()
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_SHUTDOWN_RENDERER);
	std::cout << "shutdown renderer!\n";
	Com_Printf(CON_CHANNEL_CONSOLEONLY, "shutting down renderer\n");

	if (ImGui::GetCurrentContext()) {
		Com_Printf(CON_CHANNEL_CONSOLEONLY, "also removing imgui context\n");
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	return detour_func.cast_call<void(*)()>();

}
void Renderer::R_RecoverLostDevice()
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_RECOVER_LOST_DEVICE);

	if (ImGui::GetCurrentContext()) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	if (detour_func.cast_call<bool(*)()>()) {
		std::cout << "recover device\n";

	}
	return;
}
LRESULT __stdcall Renderer::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_WNDPROC);

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return 1l;
	}

	switch (uMsg) {
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	}

	return detour_func.cast_call<LRESULT(*)(HWND, UINT, WPARAM, LPARAM)>(hWnd, uMsg, wParam, lParam);

}