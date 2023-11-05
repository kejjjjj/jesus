#pragma once

#include "pch.hpp"

class Gui_MainCategory
{
public:
	Gui_MainCategory(LPDIRECT3DTEXTURE9 icon, const std::string& previewtext) : thumbnail(icon), preview_text(previewtext), resources(Resources::getInstance()) {}

	void append_subcategory(std::unique_ptr<Gui_SubCategory> category) 
	{ 
		categories.push_back(std::move(category)); 
		if(categories.size() == 1) 
			active_category = categories.front().get(); 
	}

	void render(const ivec2& pos, const ivec2& maxs, Gui_MainCategory** active);
	void render_subcategories(const ivec2& pos, const ivec2& maxs);
	void set_active(Gui_SubCategory& c) noexcept { active_category = &c; }
	Gui_SubCategory* get_active() const noexcept { return active_category; };
private:
	void on_hovered(const ivec2& pos, const ivec2& maxs, Gui_MainCategory** active);
	void render_image_button(ivec2& pos, ivec2& maxs);

	LPDIRECT3DTEXTURE9 thumbnail;
	std::string preview_text;
	std::list<std::unique_ptr<Gui_SubCategory>> categories;
	Gui_SubCategory* active_category = 0;
	const ivec2 thumbnail_size = { 50, 50 };
	Resources& resources;

	Gui_MainCategory(const Gui_MainCategory&) = delete;
	Gui_MainCategory& operator=(const Gui_MainCategory&) = delete;
};

