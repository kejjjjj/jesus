#include "pch.hpp"

void Gui::initialize()
{
	decltype(auto) resources = Resources::getInstance();

	cheats_create_hardcoded();
	//automation_create_hardcoded();
	geometry_create_hardcoded();

	set_active(categories.front().get());
	
}
void Gui::menu_toggle()
{
	decltype(auto) gui = getInstance();
	gui.is_open = !gui.is_open;

	if (gui.is_open)
		return gui.open();

	gui.close();

}
void Gui::close() noexcept
{
	IN_ActivateMouse(true);
	UI_SetSystemCursorPos(320.f, 240.f);
	IN_ActivateKeyboard(true);
	ImGui::GetIO().MouseDrawCursor = false;
	is_open = false;
}
void Gui::open() noexcept
{
	IN_ActivateMouse(false);
	UI_SetSystemCursorPos(320.f, 240.f);
	IN_ActivateKeyboard(false);

	ImGui::GetIO().MouseDrawCursor = true;

	is_open = true;
}
void Gui::reset()
{
	close();
}
void Gui::render()
{
	if (!is_open)
		return;


	ImGui::Begin(WINDOW_TITLE, 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar/*ImGuiWindowFlags_AlwaysAutoResize*/);
	
	if (ImGui::IsKeyPressed(VK_ESCAPE))
		return close();

	render_topbar();
	left_categories();
	right_categories();
	bottom_categories();

	ImGui::End();

	//ImPlot::ShowDemoWindow();

}


void Gui::render_topbar()
{
	
	ImGui::Text("free space: (%ipx, %ipx)", free_space.x, free_space.y);
	ImGui::Dummy(ivec2(0, 60));

	const auto& color = ImGui::GetStyle().Colors[ImGuiCol_Separator];
	const auto c = ImColor(color.x, color.y, color.z, color.w);

	ImGui::GetWindowDrawList()->AddRectFilled(
		ivec2(pos.x, pos.y + ImGui::GetCursorPosY()-6),
		ivec2(pos.x + size.x, pos.y + ImGui::GetCursorPosY()-3),
		c);

	//ImGui::Separator();

	update_dimensions();
	clip_bounds();
}
void Gui::left_categories()
{
	ImGui::BeginGroup();
	auto& style  = ImGui::GetStyle();
	ivec2 mins = ivec2(pos.x+4, pos.y + cursor.y - style.WindowPadding.y / 2 + 1);
	ivec2 maxs = ivec2(pos.x + cursor.x + 150, pos.y + cursor.y + free_space.y - 100);
	maxs.x += style.FramePadding.x * style.WindowPadding.x;

	ImGui::GetWindowDrawList()->AddRectFilled(
		mins,
		maxs,
		IM_COL32(33, 33, 33, 255));

	left_mins = mins;
	left_maxs = maxs;

	ImGui::Dummy(ivec2(8, 0));
	ImGui::SameLine();
	ImGui::BeginChild("##11asd", ImVec2(maxs.x - mins.x - 20, maxs.y - mins.y), false, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

	ImGui::BeginGroup();

	active_category->render_subcategories(mins , maxs);

	ImGui::EndGroup();

	ImGui::EndChild();

	//float height = ImGui::GetItemRectSize().y;

	//ImGui::Dummy(ImVec2(maxs.x - mins.x, maxs.y - mins.y - height));

	//ImGui::EndChild();
	

	update_dimensions();
	clip_bounds();

	ImGui::EndGroup();

}
void Gui::bottom_categories()
{
	const auto& color = ImGui::GetStyle().Colors[ImGuiCol_Separator];
	const auto c = ImColor(color.x, color.y, color.z, color.w);
	ImGui::GetWindowDrawList()->AddRectFilled(
		ivec2(pos.x, pos.y + ImGui::GetCursorPosY() - 6),
		ivec2(pos.x + size.x, pos.y + ImGui::GetCursorPosY() - 3),
		c);

	static const size_t numCategories = categories.size();
	decltype(auto) resources = Resources::getInstance();

	if (numCategories > 0) {
		
		const auto old_font = ImGui::GetFont();

		if (auto font = resources.FindFont(BAHNSCHRIFT_M))
			ImGui::SetCurrentFont(font.value());

		const int padding = mainSize.x / 1.5f;
		ivec2 _pos = main_category_first_image_position(numCategories, padding);

		//ImGui::Dummy(ivec2(0, _pos.y - padding));
		//ImGui::Dummy(ivec2(_pos.x - padding/2, 0.f));
		//ImGui::SameLine();
		for(auto it = categories.begin(); it != categories.end(); it++){

			const auto i = std::distance(categories.begin(), it);

			size_t x_offset = ((mainSize.x) + padding) * i;
			ivec2 mins = ivec2(pos.x + _pos.x + x_offset, pos.y + _pos.y);
			ivec2 maxs = ivec2(pos.x + _pos.x + x_offset + mainSize.x, pos.y + _pos.y + mainSize.y);

			(*it)->render(mins, maxs, &active_category);
			//ImGui::SameLine();
			//ImGui::Dummy(ivec2(5, 0));
			//ImGui::SameLine();

			//ImGui::GetWindowDrawList()->AddRect(
			//	ivec2(pos.x + _pos.x + x_offset, pos.y + _pos.y), 
			//	ivec2(pos.x + _pos.x + x_offset + mainSize.x, pos.y + _pos.y + mainSize.y), IM_COL32(255, 255, 255, 255));

			//const std::string str = std::to_string(i);
			//const ivec2 tsize = ImGui::CalcTextSize(str.c_str());

			//ImGui::GetWindowDrawList()->AddText(ivec2(pos.x + _pos.x + x_offset + 32 - tsize.x/2, pos.y + _pos.y + 32 - tsize.y/2), IM_COL32(255, 255, 255, 255), str.c_str());
		}

		ImGui::SetCurrentFont(old_font);

	}

}
void Gui::right_categories()
{
	auto& style = ImGui::GetStyle();
	ivec2 mins = ivec2(left_maxs.x, left_mins.y);
	ivec2 maxs = ivec2(pos.x + size.x, left_maxs.y);
	//maxs.x += style.FramePadding.x * style.WindowPadding.x;

	ImGui::GetWindowDrawList()->AddRectFilled(
		mins,
		maxs,
		IM_COL32(28,28,28,255));

	ImGui::SameLine();

	ImGui::BeginChild("right_cat", ivec2(pos.x + size.x - 18, left_maxs.y - 5) - mins, false);


	for (auto& i : active_category->get_active()->get_items()) {

		const_cast<Gui_CategoryItems&>(*i).render();
	}


	ImGui::EndChild();


}
ivec2 Gui::main_category_first_image_position(const size_t numimages, float spacing) const noexcept
{

	const float totalwidth = (numimages * mainSize.x) + ((numimages - 1) * (numimages < 2 ? 1 : spacing));
	const float start_x = (size.x - totalwidth) / 2;
	const float start_y = (cursor.y + (free_space.y - mainSize.y) * 0.5f) - ImGui::GetFontSize()/2 - 4;

	return { int(start_x), int(start_y) };

}
void Gui::update_dimensions()
{
	pos = ImGui::GetWindowPos();
	size = ImGui::GetWindowSize();
	cursor = ImGui::GetCursorPos();
	absolute_maxs = ivec2(cgs->refdef.width, cgs->refdef.height);
	free_space = ivec2(size.x - cgs->refdef.x - cursor.x, size.y - cgs->refdef.y - cursor.y);
	//free_space -= cursor;
}
void Gui::clip_bounds()
{
	if (size.x > absolute_maxs.x)
		ImGui::SetWindowSize(ivec2(absolute_maxs.x, size.y));
	if (size.y > absolute_maxs.y)
		ImGui::SetWindowSize(ivec2(size.x, absolute_maxs.y));

	if (pos.x < NULL)
		ImGui::SetWindowPos(ivec2(0, pos.y));
	if (pos.y < NULL)
		ImGui::SetWindowPos(ivec2(pos.x, 0));

	else if (pos.x + size.x > absolute_maxs.x)
		ImGui::SetWindowPos(ivec2(absolute_maxs.x - size.x, pos.y));
	else if (pos.y + size.y > absolute_maxs.y)
		ImGui::SetWindowPos(ivec2(pos.x, absolute_maxs.y - size.y));
}