#pragma once

#include "pch.hpp"

class Gui_SubCategory
{
public:
	Gui_SubCategory(const std::string& display_text) : preview_text(display_text){}

	void render(const ivec2& pos, const ivec2& maxs, bool active, float y_offs, Gui_SubCategory** currently_active);

	void append_itemlist(std::unique_ptr<Gui_CategoryItems> c) { items.push_back(std::move(c)); }
	auto& get_items() const noexcept { return items; }
	const int rect_height = 42;
private:

	void render_inactive(const ivec2& pos, const ivec2& maxs, float y_offs, Gui_SubCategory** currently_active);
	void render_active(const ivec2& pos, const ivec2& maxs, float y_offs);
	
	std::list<std::unique_ptr<Gui_CategoryItems>> items;

	std::string preview_text;
	
	Gui_SubCategory(const Gui_SubCategory&) = delete;
	Gui_SubCategory& operator=(const Gui_SubCategory&) = delete;
};
