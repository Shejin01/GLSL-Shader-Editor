#pragma once

#include "../Math/Precision.h"
#include "../Logger/Logger.h"
#include <sstream>
#include <fstream>
#include <vector>

namespace File {
	String LoadFromFile(String filepath);
	std::vector<String> LoadLinesFromFile(String filepath);
	bool SaveToFile(String filepath, String text);
}