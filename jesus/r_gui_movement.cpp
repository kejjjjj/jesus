#include "pch.hpp"

void Gui::movement_create_hardcoded()
{

	decltype(auto) resources = Resources::getInstance();

	std::unique_ptr<Gui_MainCategory> category = std::make_unique<Gui_MainCategory>(resources.FindTexture("movement").value(), "Movement");
	std::unique_ptr<Gui_SubCategory> subcategory_automation = std::make_unique<Gui_SubCategory>("Strafing");
	std::unique_ptr<Gui_SubCategory> subcategory_fps = std::make_unique<Gui_SubCategory>("FPS");
	std::unique_ptr<Gui_SubCategory> subcategory_jumping = std::make_unique<Gui_SubCategory>("Jumping");

	movement_create_strafing(subcategory_automation);
	movement_create_fps(subcategory_fps);
	movement_create_jumping(subcategory_jumping);

	append_category(std::move(category));

	categories.back()->append_subcategory(std::move(subcategory_automation));
	categories.back()->append_subcategory(std::move(subcategory_fps));
	categories.back()->append_subcategory(std::move(subcategory_jumping));

}
void Gui::movement_create_strafing(std::unique_ptr<Gui_SubCategory>& category)
{
	EvarTable& instance = EvarTable::getInstance();

	std::unique_ptr<Gui_CategoryItems> items_strafing = std::make_unique<Gui_CategoryItems>("General");

	const auto strafebot = instance.add_variable<bool>("Strafebot", false);
	const auto auto_para = instance.add_variable<bool>("Auto Para", false);

	items_strafing->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(strafebot, "Optimal acceleration for strafing"))));
	items_strafing->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(auto_para, "force strafing inside the 333fps zone when possible"))));


	category->append_itemlist(std::move(items_strafing));



}
void Gui::movement_create_fps(std::unique_ptr<Gui_SubCategory>& category)
{
	EvarTable& instance = EvarTable::getInstance();

	std::unique_ptr<Gui_CategoryItems> items = std::make_unique<Gui_CategoryItems>("FPS");

	const auto autofps = instance.add_variable<bool>("AutoFPS", false);

	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(autofps, "Automatically switches to the best FPS for acceleration"))));

	category->append_itemlist(std::move(items));

}
void Gui::movement_create_jumping(std::unique_ptr<Gui_SubCategory>& category)
{
	EvarTable& instance = EvarTable::getInstance();

	std::unique_ptr<Gui_CategoryItems> items_strafing = std::make_unique<Gui_CategoryItems>("General");

	const auto easy_bounces = instance.add_variable<bool>("Easy Bounces", false);
	//const auto auto_para = instance.add_variable<bool>("Auto Para", false);

	items_strafing->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(easy_bounces, "Makes hitting difficult-to-hit bounces a lot easier"))));


	category->append_itemlist(std::move(items_strafing));
}