#include "pch.hpp"

void brush_geom_find_with_filter(const bool newState)
{

	if (!newState) {
		CM_ClearBrushes();
		Com_Printf(CON_CHANNEL_OBITUARY, "^2clearing!\n");
		return;
	}

	const auto filter = find_evar<char>("Filter");
	float planes[40][4]{};

	for (int i = 0; i < cm->numBrushes; i++) {

		if (std::string(cm->materials[cm->brushes[i].axialMaterialNum[0][0]].material).find(filter->get_array()) == std::string::npos)
			continue;

		GetBrushPolys(&cm->brushes[i], planes);

	}

	Com_Printf(CON_CHANNEL_OBITUARY, "^2%s!\n", filter->get_array());


}
void brush_geom_change_filter()
{
	CM_ClearBrushes();
	brush_geom_find_with_filter(true);
}
void terrain_geom_find_with_filter(const bool newState)
{

	if (!newState) {
		cm_terrainpoints.clear();
		Com_Printf(CON_CHANNEL_OBITUARY, "^2clearing!\n");
		return;
	}

	const auto filter = find_evar<char>("Filter##01");

	for (int i = 0; i < cm->numLeafs; i++) {
		CM_GetTerrainTriangles(&cm->leafs[i], filter->get_array());

	}


	//Com_Printf(CON_CHANNEL_OBITUARY, "^2%s -> [%s]\n", filter->get_array(), cm->materials[cm_terrainpoints.front().aabb->materialIndex].material);


}
void terrain_geom_change_filter()
{
	cm_terrainpoints.clear();
	terrain_geom_find_with_filter(true);
}
void Gui::geometry_create_hardcoded()
{
	decltype(auto) resources = Resources::getInstance();

	std::unique_ptr<Gui_MainCategory> category = std::make_unique<Gui_MainCategory>(resources.FindTexture("clipmap").value(), "Clip Map");
	std::unique_ptr<Gui_SubCategory> subcategory = std::make_unique<Gui_SubCategory>("Brushes");
	std::unique_ptr<Gui_SubCategory> subcategory_terrain = std::make_unique<Gui_SubCategory>("Terrain");
	std::unique_ptr<Gui_SubCategory> subcategory_preferences = std::make_unique<Gui_SubCategory>("Preferences");


	geometry_create_clipmap(subcategory);
	geometry_create_terrain(subcategory_terrain);
	geometry_create_preferences(subcategory_preferences);

	append_category(std::move(category));
	categories.back()->append_subcategory(std::move(subcategory));
	categories.back()->append_subcategory(std::move(subcategory_terrain));
	categories.back()->append_subcategory(std::move(subcategory_preferences));

}

void Gui::geometry_create_clipmap(std::unique_ptr<Gui_SubCategory>& category)
{
	char coll_buf[128] = "clip";
	EvarTable& instance = EvarTable::getInstance();

	//Gui_CategoryItems items("Brushes");

	std::unique_ptr<Gui_CategoryItems> items = std::make_unique<Gui_CategoryItems>("Brushes");


	const auto showcollision = instance.add_variable<bool>("Show Collisions", false);
	const auto showcollision_filter = instance.add_array<char>("Filter", coll_buf, 128);

	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(showcollision, "Draws the outlines of each collision brush", brush_geom_find_with_filter))));
	items->append_item(std::move(std::make_unique<Gui_ItemInputText>(Gui_ItemInputText(showcollision_filter, "Only draws materials that include this substring", 128, brush_geom_change_filter))));

	category->append_itemlist(std::move(items));




}
void Gui::geometry_create_terrain(std::unique_ptr<Gui_SubCategory>& category)
{
	EvarTable& instance = EvarTable::getInstance();
	std::unique_ptr<Gui_CategoryItems> items = std::make_unique<Gui_CategoryItems>("Terrain");
	char coll_buf[128] = "clip";


	const auto showterrain = instance.add_variable<bool>("Show Terrain", false);
	const auto showterrain_filter = instance.add_array<char>("Filter##01", coll_buf, 128);
	const auto showterrain_unwalkable = instance.add_variable<bool>("Unwalkable Edges", false);

	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(showterrain, "Draws the outlines of each terrain piece", terrain_geom_find_with_filter))));
	items->append_item(std::move(std::make_unique<Gui_ItemInputText>(Gui_ItemInputText(showterrain_filter, "Only draws materials that include this substring", 128, terrain_geom_change_filter))));
	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(showterrain_unwalkable, "Show terrain edges you can't walk on (possible new bounces)"))));

	category->append_itemlist(std::move(items));

}
void Gui::geometry_create_preferences(std::unique_ptr<Gui_SubCategory>& category)
{
	EvarTable& instance = EvarTable::getInstance();

	//Gui_CategoryItems items("General");
	std::unique_ptr<Gui_CategoryItems> items = std::make_unique<Gui_CategoryItems>("General");


	const auto only_bounces = instance.add_variable<bool>("Only Bounces", false);
	const auto depth_test = instance.add_variable<bool>("Depth Test", false);
	const auto draw_distance = instance.add_variable<float>("Draw Distance", 2000.f);

	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(only_bounces, "Only renders windings that can be bounced"))));
	items->append_item(std::move(std::make_unique<Gui_ItemCheckbox>(Gui_ItemCheckbox(depth_test, "Don't render through walls"))));
	items->append_item(std::move(std::make_unique<Gui_ItemSliderFloat>(Gui_ItemSliderFloat(draw_distance, "Maximum distance to show collision surfaces", 0, 20000))));

	category->append_itemlist(std::move(items));

}