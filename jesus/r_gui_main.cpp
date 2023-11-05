#include "pch.hpp"

void Gui_MainCategory::render(const ivec2& pos, const ivec2& maxs, Gui_MainCategory** active)
{
	render_image_button(const_cast<ivec2&>(pos), const_cast<ivec2&>(maxs));
	
	if (*active == this) {
		ImGui::GetWindowDrawList()->AddRectFilled(pos, maxs, IM_COL32(255, 255, 255, 55), ImGui::GetStyle().WindowRounding);
		return;
	}
	on_hovered(pos, maxs, active);
}
void Gui_MainCategory::render_image_button(ivec2& pos, ivec2& maxs)
{
	//ivec2 pos = ImGui::GetCursorPos();
	ImGui::GetWindowDrawList()->AddImage(thumbnail, pos, maxs);

	const auto old_font = ImGui::GetFont();

	if (auto font = resources.FindFont(BAHNSCHRIFT_S))
		ImGui::SetCurrentFont(font.value());

	const ivec2 textSize = ImGui::CalcTextSize(preview_text.c_str());
	int centerX = pos.x + (maxs.x - pos.x);
	int offset = thumbnail_size.x - textSize.x;

	const ivec2 textPos(centerX - textSize.x - offset/2, maxs.y+8);

	ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(158,158,158,255), preview_text.c_str());

	pos.x -= 16;
	pos.y -= textSize.y/2-2;
	maxs.x += 16;
	maxs.y += textSize.y + 18;
	//ImGui::ImageButton(thumbnail, thumbnail_size);


}
void Gui_MainCategory::on_hovered(const ivec2& pos, const ivec2& maxs, Gui_MainCategory** active)
{
	if (ImGui::IsMouseHoveringRect(pos, maxs, true)) {
		ImGui::GetWindowDrawList()->AddRectFilled(pos, maxs, IM_COL32(255, 255, 255, 55), ImGui::GetStyle().WindowRounding);
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			*active = this;
			Com_Printf(CON_CHANNEL_OBITUARY, "^2%s\n", preview_text.c_str());

		}
	}
}

void Gui_MainCategory::render_subcategories(const ivec2& pos, const ivec2& maxs)
{
	ivec2 permins = pos;
	ivec2 permaxs = ivec2(maxs.x, pos.y + 32);

	int offs = 0;

	for (auto& i : categories) {
		i->render(permins, permaxs, active_category == i.get(), offs, &active_category);
		permins.y += i->rect_height;
		permaxs.y += i->rect_height;
		offs += i->rect_height + 10;
	}

}