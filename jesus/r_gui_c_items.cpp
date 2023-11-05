
#include "pch.hpp"


int Gui_CategoryItems::render(int horz_offset)
{
	int horz_size = 200;
	ImGui::BeginGroup();
	ImGui::BeginChild("huuuh", ivec2(horz_size, 400), true);
	child_mins = ImGui::GetWindowPos();
	child_mins.x += horz_offset;
	render_titlebar(horz_offset);
	
	decltype(auto) resources = Resources::getInstance();

	const auto old_font = ImGui::GetFont();
	if (auto font = resources.FindFont(ARIAL_S))
		ImGui::SetCurrentFont(font.value());

	render_body(horz_offset);

	ImGui::EndChild();
	ImGui::EndGroup();

	ImGui::SetCurrentFont(old_font);
	//ImGui::Dummy(ivec2(horz_size, 0));
	ImGui::SameLine();

	return horz_size + 10;
}
void Gui_CategoryItems::render_titlebar(int horz_offset)
{
	
	auto window = ImGui::GetCurrentWindow();
	ImRect title_bar_rect = window->TitleBarRect();
	title_bar_rect.Min.x += horz_offset;
	title_bar_rect.Max.x += horz_offset;

	ivec2 p = title_bar_rect.Min;
	auto wnd = ImGui::FindWindowByName(WINDOW_TITLE)->DrawList;
	const auto& color = ImGui::GetStyle().Colors[ImGuiCol_Separator];
	const auto c = ImColor(color.x, color.y, color.z, color.w);

	wnd->AddRectFilled(title_bar_rect.Min, ivec2(title_bar_rect.Max.x, title_bar_rect.Max.y + 32), IM_COL32(42, 42, 42, 255), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersTop);

	wnd->AddRectFilled(
		ivec2(p.x, p.y + 32),
		ivec2(title_bar_rect.Max.x, p.y + 32 + 3),
		c);

	wnd->AddText(ivec2(title_bar_rect.Min.x + 10, title_bar_rect.Min.y + 16 - ImGui::GetFontSize()/2), IM_COL32(255, 255, 255, 255), title.c_str());
	ImGui::Dummy(ivec2(0, title_bar_rect.Max.y + 32 - title_bar_rect.Min.y));

	body_mins = ivec2(p.x, p.y + 32 + 3);
}
void Gui_CategoryItems::render_body(int horz_offset)
{
	//static bool cb;
	//static float slider = 1;
	//static const char* items[] = { "first", "second", "third" };
	//int itemIndex = 0;

	//ImGui::SetCursorPos(ivec2(body_mins.x - child_mins.x + horz_offset, body_mins.y - child_mins.y));

	auto wnd = ImGui::FindWindowByName(WINDOW_TITLE)->DrawList;
	wnd->AddRectFilled(body_mins, ivec2(body_mins.x + 200, body_mins.y + (400 - (body_mins.y - child_mins.y))), IM_COL32(33, 33, 33, 255), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersBottom);

	for (auto& i : items) {
		switch (i->get_type()) {
		case GuiItemType::CHECKBOX:
			dynamic_cast<Gui_ItemCheckbox*>(i.get())->render();
			break;
		case GuiItemType::INPUTBOX:
			dynamic_cast<Gui_ItemInputText*>(i.get())->render();
			break;
		case GuiItemType::SLIDER_FLOAT:
			dynamic_cast<Gui_ItemSliderFloat*>(i.get())->render();
			break;
		}
	}


	//ImGui::Checkbox2("checkbox1", &cb);
	//ImGui::SetNextItemWidth(150);
	//ImGui::SliderFloat_2("slider1", &slider, 0, 100, "%.3f");
	//ImGui::SetNextItemWidth(150);
	//ImGui::Combo("combo", &itemIndex, items, 3);
}