#include "pch.hpp"


void Gui::cheats_create_hardcoded()
{
	decltype(auto) resources = Resources::getInstance();

	std::unique_ptr<Gui_MainCategory> category = std::make_unique<Gui_MainCategory>(resources.FindTexture("cheats").value(), "Cheats");
	std::unique_ptr<Gui_SubCategory> subcategory_visuals = std::make_unique<Gui_SubCategory>("Visuals");
	std::unique_ptr<Gui_SubCategory> subcategory_aim = std::make_unique<Gui_SubCategory>("Aim");

	cheats_create_visuals(subcategory_visuals);
//	automation_create_fps(subcategory_fps);

	append_category(std::move(category));

	categories.back()->append_subcategory(std::move(subcategory_visuals));
	//categories.back()->append_subcategory(std::move(subcategory_fps));
}
void Gui::cheats_create_visuals(std::unique_ptr<Gui_SubCategory>& category)
{
	EvarTable& instance = EvarTable::getInstance();

	std::unique_ptr<Gui_CategoryItems> items = std::make_unique<Gui_CategoryItems>("Visuals");

	const auto player_names = instance.add_variable<bool>("Player Names", false);
	const auto weapons = instance.add_variable<bool>("Player Weapons", false);
	const auto killable_enemy = instance.add_variable<bool>("Killable enemy", false);
	const auto hitboxes = instance.add_variable<bool>("Hitboxes", false);
	const auto compass = instance.add_variable<bool>("360 Compass", false);

	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(player_names, "Render player names"))));
	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(weapons, "Render player weapons"))));
	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(killable_enemy, "Shows a hint if you can kill a player from your current position"))));
	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(hitboxes, "Render player hitboxes"))));
	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(compass, "Show enemies around you"))));

	category->append_itemlist(std::move(items));
}