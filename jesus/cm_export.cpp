#include "pch.hpp"

struct export_brush
{
	fvec3 points[3];
};

void CM_WriteHeader(std::ofstream& f)
{
	f << "iwmap 4\n";
	f << "\"000_Global\" flags  active\n";
	f << "\"The Map\" flags\n";
	f << "// entity 0\n{\n";
	f << "\"contrastGain\" " "\"0.125\"" << '\n';
	f << "\"diffusefraction\" " "\"0.5\"" << '\n';
	f << "\"_color\" " "\"0.2 0.27 0.3 1\"" << '\n';
	f << "\"sunlight\" " "\"1\"" << '\n';
	f << "\"sundirection\" " "\"-30 -95 0\"" << '\n';
	f << "\"sundiffusecolor\" " "\"0.2 0.27 0.3 1\"" << '\n';
	f << "\"suncolor\" " "\"0.2 0.27 0.3 1\"" << '\n';
	f << "\"ambient\" " "\".1\"" << '\n';
	f << "\"bouncefraction\" " "\".7\"" << '\n';
	f << "\"classname\" \"worldspawn\"\n";
}
void CM_WriteBrush(std::ofstream& o, const showcol_brush& b, int index)
{
	o << "// brush " << index << '\n';
	o << "{\n";

	std::vector<export_brush> brushside;

	for (auto& winding : b.windings) {
		brushside.push_back({ winding.points[0], winding.points[1], winding.points[2] });
	}

	//std::swap(brushside[3], brushside[4]);
	//std::swap(brushside[1], brushside[3]);
	//std::swap(brushside[0], brushside[1]);

	for (auto& b : brushside) {
		for (int i = 0; i < 3; i++) {
			o << std::format(" ( {} {} {} )", b.points[i].x, b.points[i].y, b.points[i].z);
		}
		o << " caulk 128 128 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0\n";

	}

	//size_t iter = 0;
	//for (auto& winding : b.windings) {

	//	for (int i = 0; i < 3; i++) {
	//		o << std::format(" ( {} {} {} )", winding.points[i].x, winding.points[i].y, winding.points[i].z);
	//	}
	//		o << " caulk 128 128 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0\n";
	//	

	//}

	o << "}\n";



}
void CM_WriteAllBrushes(std::ofstream& o) 
{
	if (s_brushes.empty()) {
		return FatalError("s_brushes.empty()");
	}

	int brushIndex = 0;

	for (auto& brush : s_brushes) {
		CM_WriteBrush(o, brush, brushIndex++);
		break; //only write one
	}

	//end brushes
	o << "}\n";


}

void CM_WriteInOrder(std::ofstream& o) 
{
	CM_WriteHeader(o);
	CM_WriteAllBrushes(o);


	
}

void CM_MapExport()
{
	if (NOT_SERVER)
		return;

	std::string path = fs::exe_path() + "\\map_source\\kej";
	std::string full_path = path + "\\" + Dvar_FindMalleableVar("mapname")->current.string + ".map";

	if (!fs::directory_exists(path)) {
		if (!fs::create_directory(path)) {
			return FatalError(std::format("couldn't create the path \"{}\"\reason: {}", path, fs::get_last_error()));
		}
	}

	fs::create_file(full_path);

	std::ofstream o(full_path, static_cast<int>(fs::fileopen::FILE_OUT));

	if (!o.is_open()) {
		FatalError(std::format("an error occurred while trying to open \"{}\"!\nreason: {}", full_path, fs::get_last_error()));
		return;
	}

	CM_WriteInOrder(o);


	o.close();

}