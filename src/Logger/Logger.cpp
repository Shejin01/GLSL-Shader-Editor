#include "Logger.h"

namespace Logger {
	void Log(String tags, String message) {
		std::cout << '[' << tags << ']' << ' ' << message;
	}
	void LogLine(String tags, String message) {
		std::cout << '[' << tags << ']' << ' ' << message << '\n';
	}
}