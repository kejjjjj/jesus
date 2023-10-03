#include "pch.hpp"

template<typename T>
void IO_WriteData(std::ofstream& f, const T& data);

template<typename T>
std::optional<T> IO_ReadBlock(std::fstream& f, size_t amount_of_bytes = 0);

void MovementRecorder::Save2File()
{
	if (NOT_SERVER)
		return;

	const char* current_map = Dvar_FindMalleableVar("mapname")->current.string;
	std::string path = std::string("recorder\\") + current_map;
	
	CG_CreateSubdirectory(path);

	path += "\\" + std::to_string(cgs->predictedPlayerState.speed);

	CG_CreateSubdirectory(path);

	path += "\\" +  std::to_string(fs::files_in_directory(path).size() + 1) + ".kej";

	std::string real_path = fs::root_path() + "\\" + path;

	std::ofstream o(real_path, static_cast<int>(fs::fileopen::FILE_OUT));

	if (!o.is_open()) {
		FatalError(std::format("an error occurred while trying to open \"{}\"!\nreason: {}", real_path, fs::get_last_error()));
		return;
	}
	
	recording_io_data data;

	data.moveSpeedScale = cgs->predictedPlayerState.moveSpeedScaleMultiplier;

	IO_WriteData<float>(o, cgs->predictedPlayerState.moveSpeedScaleMultiplier);
	
	for (auto& data : recorder.data) {
		IO_WriteData<playback_cmd>(o, data);
	}


	o.close();
	

}
void MovementRecorder::LoadRecordings(const std::string& mapname)
{
	int speed = cgs->predictedPlayerState.speed;
	std::string path = fs::root_path() + "\\recorder\\" + mapname + "\\" + std::to_string(speed);

	auto files = fs::files_in_directory(path);

	std::for_each(files.begin(), files.end(), [this](const std::string& file) 
		{

			fs::reset();

			std::fstream f(file);

			if (!f.is_open()) {
				FatalError(std::format("an error occurred while trying to open \"{}\"!\nreason: {}", file, fs::get_last_error()));
				return;
			}

			auto data = ReadRecording(f, file);

			if (data.moveSpeedScale == cgs->predictedPlayerState.moveSpeedScaleMultiplier)
				playbacks.push_back(std::move(std::unique_ptr<Playback>(new Playback(data.data))));

			f.close();

		});

	std::cout << "total loaded: " << playbacks.size() << '\n';

	if(playbacks.empty() == false)
		playback.data_original = playbacks.front()->data_original;

}

recording_io_data MovementRecorder::ReadRecording(std::fstream& f, const std::string& file)
{
	recording_io_data io;
	io.moveSpeedScale = IO_ReadBlock<float>(f).value();

	//fs::get(f);


	while (f.good() && !f.eof()) {

		if(auto v = IO_ReadBlock<playback_cmd>(f))
			io.data.push_back(v.value());
		//fs::get(f);

	}
	return io;

}

template<typename T>
void IO_WriteData(std::ofstream& f, const T& data)
{
	DWORD base = (DWORD)&data;
	f << '[';
	for (int i = 0; i < sizeof(T); i += 1) {
		std::stringstream ss;
		std::string s;
		ss << std::hex << (int)(*(BYTE*)(base + i));

		if ((s = ss.str()).size() == 1)
			s.insert(s.begin(), '0');

		f << s;

	}
	f << "]";
}

template<typename T>
std::optional<T> IO_ReadBlock(std::fstream& f, size_t amount_of_bytes)
{
	T data{ };
	char ch = fs::get(f);

	if (f.eof())
		return std::nullopt;

	if (ch != '[') {
		FatalError(std::format("std::optional<T> Prediction::IO_ReadBlock(): expected {} instead of {}", '[', ch));
		return std::nullopt;
	}

	size_t bytes_read = 0;

	if (!amount_of_bytes)
		amount_of_bytes = sizeof(T);

	DWORD base = (DWORD)(&data);

	do {

		std::string hex = "0x";

		for (int i = 0; i < 2; i++) {

			if (f.eof() || !f.good()) {
				FatalError("std::optional<T> Prediction::IO_ReadBlock(): unexpected end of file");
				return std::nullopt;
			}

			ch = fs::get(f);

			if (bytes_read == amount_of_bytes && ch != ']') {
				FatalError(std::format("bytes_read ({}) == sizeof(T) ({}) && ch != ']' ({})", bytes_read, sizeof(T), ch));
				return std::nullopt;
			}
			else if (bytes_read == amount_of_bytes && ch == ']') {
				//fs::get(f); //skip the newline
				return data;
			}

			if (!IsHex(ch)) {
				FatalError("std::optional<T> Prediction::IO_ReadBlock(): unexpected end of file");
				return std::nullopt;
			}
			hex.push_back(ch);


		}

		//here it HAS to be from 0 to 255
		auto hex_byte = std::strtol(hex.c_str(), NULL, 0);
		*(BYTE*)base = (BYTE)hex_byte;

		base += 1;
		bytes_read++;

	} while (true);

	FatalError("std::optional<T> Prediction::IO_ReadBlock(): unexpected end of file");
	return std::nullopt;
}