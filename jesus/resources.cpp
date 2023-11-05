#include "pch.hpp"

LPDIRECT3DTEXTURE9 file2texture(const std::string& source)
{
	LPDIRECT3DTEXTURE9 tex;
	const auto result = D3DXCreateTextureFromFile(cg::dx->device, source.c_str(), &tex);

	if (result != S_OK) {
		FatalError(std::format("file2texture(): \"{}\" failed with {}", source, result));
		return 0;
	}

	return tex;
}

void Resources::initialize()
{
	create_subdirectory("");
	create_subdirectory("resources");
	create_subdirectory("resources\\fonts");
	create_subdirectory("resources\\images");
	create_subdirectory("recorder");

	//create_subdirectory("prediction_runs");


	recreate_materials();
}

void Resources::create_subdirectory(const std::string& name) const noexcept
{
	const std::string full_path = fs::exe_path() + "\\Jesus\\" + name;

	if (!fs::directory_exists(full_path)) {
		if (!fs::create_directory(full_path)) {
			FatalError(std::format("unable to create the following directory: '{}'", full_path));
			return;
		}
	}
}
void Resources::recreate_materials()
{
	load_font(BAHNSCHRIFT_M, "bahnschrift.ttf", 24.f);
	load_font(BAHNSCHRIFT_S, "bahnschrift.ttf", 19.5f);

	load_font(ARIAL_S, "Artifakt Element Regular.ttf", 22.f);


	//load_image("visuals", "visuals.png");
	//load_image("automation", "automation.png");
	load_image("clipmap", "clipmap.png");
	load_image("cheats", "cheats.png");
	load_image("movement", "movement.png");

}
void Resources::load_font(const char* name, const std::string& file_name, const float scale)
{
	ImGuiIO* io = &ImGui::GetIO();
	if (!io) {
		FatalError("load_font(): imgui context not yet created!");
		return;
	}

	const std::string full_path = fs::root_path() + "\\resources\\fonts";
	const std::string c_str = std::string(full_path + "\\" + file_name);

	if (!fs::file_exists(c_str)) {
		std::cout << "filename: " << c_str.c_str() << '\n';
		FatalError(std::format("unable to find the font: '{}'", c_str));
		return;
	}

	auto font = io->Fonts->AddFontFromFileTTF(c_str.c_str(), scale);

	if (!font) {
		std::cout << "font: " << std::quoted(c_str) << " does not exist!\n";
		FatalError(std::format("font '{}' does not exist!", c_str));
		return;
	}

	fonts.push_back(std::make_pair(font, name));

	std::cout << "font: " << std::quoted(name) << " added!\n";

}
std::optional<ImFont*> Resources::FindFont(const std::string& name) const noexcept
{
	for (const auto& i : fonts) {
		if (!name.compare(i.second))
			return i.first;
	}
	FatalError(std::format("FindFont(): couldn't find font \"{}\"", name));
	return std::nullopt;
}
std::optional<LPDIRECT3DTEXTURE9> Resources::FindTexture(const std::string& name) const noexcept
{
	for (const auto& i : images) {
		if (!name.compare(i.second))
			return i.first;
	}
	FatalError(std::format("FindTexture(): couldn't find texture \"{}\"", name));
	return std::nullopt;
}
void Resources::load_image(const char* name, const std::string& file_name)
{
	const std::string full_path = fs::root_path() + "\\resources\\images";
	const std::string c_str = std::string(full_path + "\\" + file_name);

	images.push_back(std::make_pair(file2texture(c_str), name));

}
void Resources::free_images()
{
	for (auto& i : images) {
		i.first->Release();
		i.first = 0;
	}
}