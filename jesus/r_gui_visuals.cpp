#include  "pch.hpp"
//void Gui::visuals_create_hardcoded()
//{
//	decltype(auto) resources = Resources::getInstance();
//
//	std::unique_ptr<Gui_MainCategory> category = std::make_unique<Gui_MainCategory>(resources.FindTexture("visuals").value(), "Visuals");
//	std::unique_ptr<Gui_SubCategory> subcategory = std::make_unique<Gui_SubCategory>("Anglehelpers");
//
//	visuals_create_strafes(subcategory);
//
//	append_category(std::move(category));
//	active_category->append_subcategory(std::move(subcategory));
//	
//}
//void Gui::visuals_create_strafes(std::unique_ptr<Gui_SubCategory>& category)
//{
//	EvarTable& instance = EvarTable::getInstance();
//
//	std::unique_ptr<Gui_CategoryItems> items = std::make_unique<Gui_CategoryItems>("Strafing");
//
//	
//	const auto aimtrainer = instance.add_variable<bool>("Aim trainer", false);
//	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(aimtrainer, "creates a ball that you have to look at with your cursor"))));
//
//	category->append_itemlist(std::move(items));
//
//
//
//}