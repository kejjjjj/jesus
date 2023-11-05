#pragma once

#include "pch.hpp"

constexpr const char* BAHNSCHRIFT_M = "bahnschrift_m";
constexpr const char* BAHNSCHRIFT_S = "bahnschrift_s";

constexpr const char* ARIAL_S = "arial_s";

class Resources
{
public:
	Resources() = default;

	static Resources& getInstance() { static Resources r; return r; }

	void initialize();

	std::optional<ImFont*> FindFont(const std::string& name) const noexcept;
	std::optional<LPDIRECT3DTEXTURE9> FindTexture(const std::string& name) const noexcept;


	void recreate_materials();

	void free_images();
	void free_fonts() noexcept { fonts.clear(); };
private:
	
	void create_subdirectory(const std::string& name) const noexcept;
	void load_font(const char* name, const std::string& file_name, const float scale);
	void load_image(const char* name, const std::string& file_name);

	std::vector<std::pair<ImFont*, const char*>> fonts;
	std::vector<std::pair<LPDIRECT3DTEXTURE9, const char*>> images;

};