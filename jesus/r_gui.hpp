#pragma once

#include "pch.hpp"

constexpr const char* WINDOW_TITLE = "##xkejj01";

class Gui
{
public:
	static Gui& getInstance() { static Gui g; return g; }

	static void menu_toggle();
	void initialize();
	void reset();

	void render();

	//returns the maxs
	void left_categories();
	void bottom_categories();
	void right_categories();

	void append_category(std::unique_ptr<Gui_MainCategory> category) {
		categories.push_back(std::move(category)); 
		if (categories.size() == 1) 
			active_category = categories.front().get(); 
	};

	ivec2 main_category_first_image_position(const size_t numimages, float spacing) const noexcept;
	void set_active(Gui_MainCategory* c) noexcept { active_category = c; }
private:



	void cheats_create_hardcoded();
	void cheats_create_visuals(std::unique_ptr<Gui_SubCategory>& category);
	void cheats_create_inputs(std::unique_ptr<Gui_SubCategory>& category);

	void movement_create_hardcoded();
	void movement_create_strafing(std::unique_ptr<Gui_SubCategory>& category);
	void movement_create_fps(std::unique_ptr<Gui_SubCategory>& category);
	void movement_create_jumping(std::unique_ptr<Gui_SubCategory>& category);

	void geometry_create_hardcoded();
	void geometry_create_clipmap(std::unique_ptr<Gui_SubCategory>& category);
	void geometry_create_terrain(std::unique_ptr<Gui_SubCategory>& category);
	void geometry_create_preferences(std::unique_ptr<Gui_SubCategory>& category);

	void render_topbar();
	void clip_bounds();

	void update_dimensions();

	void close() noexcept;
	void open() noexcept;
	bool is_open = false;

	std::list<std::unique_ptr<Gui_MainCategory>> categories;
	Gui_MainCategory* active_category = 0;
	ivec2 absolute_maxs;
	ivec2 free_space;
	ivec2 pos;
	ivec2 size;
	ivec2 cursor;

	ivec2 mainSize = { 50, 50 };

	ivec2 left_mins;
	ivec2 left_maxs;

};