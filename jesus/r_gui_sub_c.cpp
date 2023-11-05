#include "pch.hpp"

void Gui_SubCategory::render(const ivec2& pos, const ivec2& maxs, bool active, float y_offs, Gui_SubCategory** currently_active)
{
	
	if(active)
		return render_active(pos, maxs, y_offs);

	render_inactive(pos, maxs, y_offs, currently_active);
}

void Gui_SubCategory::render_inactive(const ivec2& pos, const ivec2& maxs, float y_offs, Gui_SubCategory** currently_active)
{
	auto window = ImGui::FindWindowByName(WINDOW_TITLE);
	const auto& style = ImGui::GetStyle();
	const ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
	const int offs = rect_height - 32;
	const int y = text_pos.y + y_offs + ImGui::GetFontSize() / 2 - ((offs) / 2.f);

	if (ImGui::IsMouseHoveringRect(ivec2(pos.x, y), ivec2(maxs.x, y + rect_height)) == false) {
		ImGui::Dummy(ivec2(0, 10));
		ImGui::TextColored(ImColor(0.5f, 0.5f, 0.5f, 1.f), preview_text.c_str());
		ImGui::Dummy(ivec2(0, offs-4));

		return;
	}

	ImGradient grad;

	grad.addMark(0.f, ImColor(170, 170, 170, 25));
	grad.addMark(0.5f, ImColor(170, 170, 170, 5));
	grad.addMark(1.f, ImColor(170, 170, 170, 0));
	const ImVec2 sp = ivec2(pos.x, y);
	ImGui::DrawGradientBar(&grad, sp, maxs.x - pos.x, rect_height, window->DrawList);
	ImGui::Dummy(ivec2(0, 10));
	ImGui::Text(preview_text.c_str());
	ImGui::Dummy(ivec2(0, offs-4));

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		Com_Printf(CON_CHANNEL_OBITUARY, "now active: ^2%s\n", preview_text.c_str());
		*currently_active = this;

	}

	return;

}
void Gui_SubCategory::render_active(const ivec2& pos, const ivec2& maxs, float y_offs)
{
	auto window = ImGui::FindWindowByName(WINDOW_TITLE);
	const auto& style = ImGui::GetStyle();
	const ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
	const int offs = rect_height - 32;
	const int y = text_pos.y + y_offs + ImGui::GetFontSize() / 2 - ((offs) / 2.f);
	const auto& color = ImGui::GetStyle().Colors[ImGuiCol_Separator];
	const auto c = ImColor(color.x, color.y, color.z, color.w);

	ImGradient grad;

	grad.addMark(0.f, ImColor(170, 170, 170, 25));
	grad.addMark(0.5f, ImColor(170, 170, 170, 5));
	grad.addMark(1.f, ImColor(170, 170, 170, 0));
	const ImVec2 sp = ivec2(pos.x, y);
	ImGui::DrawGradientBar(&grad, sp, maxs.x - pos.x, rect_height, window->DrawList);

	window->DrawList->AddRectFilled(ivec2(pos.x, y), ivec2(pos.x + 3, y + rect_height), c, 0);
	
	ImGui::Dummy(ivec2(0, 10));
	ImGui::Text(preview_text.c_str());
	ImGui::Dummy(ivec2(0, offs-4));

	//ImGui::GetWindowDrawList()->AddText(pos, IM_COL32(255, 255, 255, 255), preview_text.c_str());
}