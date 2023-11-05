#include "pch.hpp"

void Gui::automation_create_hardcoded()
{

	decltype(auto) resources = Resources::getInstance();

	std::unique_ptr<Gui_MainCategory> category = std::make_unique<Gui_MainCategory>(resources.FindTexture("automation").value(), "Automation");
	std::unique_ptr<Gui_SubCategory> subcategory = std::make_unique<Gui_SubCategory>("Inputs");
	std::unique_ptr<Gui_SubCategory> subcategory_fps = std::make_unique<Gui_SubCategory>("FPS");

	automation_create_movement(subcategory);
	automation_create_fps(subcategory_fps);

	append_category(std::move(category));

	categories.back()->append_subcategory(std::move(subcategory));
	categories.back()->append_subcategory(std::move(subcategory_fps));

}
void Gui::automation_create_movement(std::unique_ptr<Gui_SubCategory>& category)
{
	EvarTable& instance = EvarTable::getInstance();

	std::unique_ptr<Gui_CategoryItems> items = std::make_unique<Gui_CategoryItems>("Strafing");

	const auto strafebot = instance.add_variable<bool>("Strafebot", false);
	const auto auto_para = instance.add_variable<bool>("Auto Para", false);

	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(strafebot, "Optimal acceleration for strafing"))));
	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(auto_para, "yea (only for g_speed 190)"))));

	category->append_itemlist(std::move(items));


}
void Gui::automation_create_fps(std::unique_ptr<Gui_SubCategory>& category)
{
	EvarTable& instance = EvarTable::getInstance();

	std::unique_ptr<Gui_CategoryItems> items = std::make_unique<Gui_CategoryItems>("FPS");

	const auto autofps = instance.add_variable<bool>("AutoFPS", false);

	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(autofps, "Automatically switches to the best FPS for acceleration"))));

	category->append_itemlist(std::move(items));

}