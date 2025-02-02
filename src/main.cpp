#define STB_IMAGE_IMPLEMENTATION
#include "Application.h"

int main() {
	Config config;
	Application app(config);
	app.Run();
	return 0;
}