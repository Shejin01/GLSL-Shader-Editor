#pragma once

#include "../Math/Precision.h"
#include "../Logger/Logger.h"
#include <sstream>
#include <fstream>

namespace File {
	String LoadFromFile(String filepath);
	bool SaveToFile(String filepath, String text);
}