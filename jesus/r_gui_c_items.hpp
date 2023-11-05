#pragma once

#include "pch.hpp"

enum class GuiItemType
{
	CHECKBOX,
	INPUTBOX,
	SLIDER_FLOAT
};

class Gui_ItemBase
{
public:
	virtual ~Gui_ItemBase() = default;
	virtual GuiItemType get_type() const = 0;

};

class Gui_ItemCheckbox : public Gui_ItemBase
{

public:
	~Gui_ItemCheckbox() = default;
	Gui_ItemCheckbox() = delete;
	Gui_ItemCheckbox(EvarBase* v, const char* _tooltip, std::optional<std::function<void(bool)>> _event=std::nullopt) : linked_variable(v), tooltip(_tooltip) {


		if (_event.has_value())
			activation_event = _event.value();
	
	}
	GuiItemType get_type() const override {
		return GuiItemType::CHECKBOX;
	}

	void render() {
		auto data = linked_variable->get_raw();
		if (ImGui::Checkbox2(linked_variable->get_name().c_str(), (bool*)data) && activation_event)
			activation_event(*(bool*)data);
		ImGui::Tooltip(tooltip);
	}

private:
	EvarBase* linked_variable = 0;
	const char* tooltip;
	std::function<void(bool)> activation_event;

};

class Gui_ItemInputText : public Gui_ItemBase
{

public:
	~Gui_ItemInputText() = default;
	Gui_ItemInputText() = delete;
	Gui_ItemInputText(EvarBase* v, const char* _tooltip, size_t bufflen, std::optional<std::function<void()>> _event = std::nullopt) : linked_variable(v), tooltip(_tooltip) {

		if (!v->is_array())
			FatalError(std::format("Gui_ItemInputText(): expected {} to be an array", v->get_name()));

		if (bufflen < 1)
			FatalError("Gui_ItemInputText(): buffer length < 1");

		if (_event.has_value())
			activation_event = _event.value();

		bufsize = bufflen;

	}
	GuiItemType get_type() const override {
		return GuiItemType::INPUTBOX;
	}

	void render() {
		auto data = linked_variable->get_raw();
		ImGui::SetNextItemWidth(100);
		if(ImGui::InputText(linked_variable->get_name().c_str(), (char*)data, bufsize) && activation_event)
			activation_event();

		ImGui::Tooltip(tooltip);
	}

private:
	EvarBase* linked_variable = 0;
	const char* tooltip;
	std::function<void()> activation_event;
	size_t bufsize = 0;
};
class Gui_ItemSliderFloat : public Gui_ItemBase
{

public:
	~Gui_ItemSliderFloat() = default;
	Gui_ItemSliderFloat() = delete;
	Gui_ItemSliderFloat(EvarBase* v, const char* _tooltip, float _min, float _max, std::optional<std::function<void()>> _event = std::nullopt) : linked_variable(v), tooltip(_tooltip), min(_min), max(_max) {

		if (min >= max)
			FatalError(std::format("Gui_ItemSliderFloat(): min >= max"));

		if (_event.has_value())
			activation_event = _event.value();
		
	}
	GuiItemType get_type() const override {
		return GuiItemType::SLIDER_FLOAT;
	}

	void render() {
		auto data = linked_variable->get_raw();
		ImGui::SetNextItemWidth(100);
		if (ImGui::SliderFloat(linked_variable->get_name().c_str(), (float*)data, min, max) && activation_event)
			activation_event();

		ImGui::Tooltip(tooltip);
	}

private:
	EvarBase* linked_variable = 0;
	const char* tooltip;
	std::function<void()> activation_event;
	size_t bufsize = 0;
	float min = 0;
	float max = 0;
};


class Gui_CategoryItems
{
public:
	Gui_CategoryItems(const std::string& _title) : title(_title) {};

	void render();

	void append_item(std::unique_ptr<Gui_ItemBase> i) { items.push_back(std::move(i)); }

private:
	Gui_CategoryItems(const Gui_CategoryItems&) = delete;
	Gui_CategoryItems& operator=(const Gui_CategoryItems&) = delete;

	void render_titlebar();
	void render_body();
	std::string title;
	std::list<std::unique_ptr<Gui_ItemBase>> items;

	ivec2 child_mins;
	ivec2 body_mins;
};