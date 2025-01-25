#include "FileLoader.h"

namespace File {
	String LoadFromFile(String filepath) {
		String text;
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			std::stringstream stream;
			file.open(filepath.c_str());
			stream << file.rdbuf();
			file.close();
			text = stream.str();
		}
		catch (std::ifstream::failure e) {
			Logger::LogLine("File Loader", String("Failed to read file from: ") + filepath + '\n' + e.what());
		}

		return text;
	}

	std::vector<String> LoadLinesFromFile(String filepath) {
		std::ifstream file;
		String line;
		std::vector<String> lines;
		file.open(filepath);

		if (file.is_open()) {
			while (std::getline(file, line)) lines.push_back(line);
			file.close();
		}
		else {
			Logger::LogLine("File Loader", String("Failed to read file from: ") + filepath);
		}

		return lines;
	}

	bool SaveToFile(String filepath, String text) {
		std::ofstream file;
		file.exceptions(std::ofstream::badbit | std::ofstream::failbit);

		bool success = true;
		try {
			file.open(filepath);
			file << text;
			file.close();
		}
		catch (std::ofstream::failure e) {
			Logger::LogLine("File Loader", String("Failed to save to file at: ") + filepath + '\n' + e.what());
			success = false;
		}

		return success;
	}
}