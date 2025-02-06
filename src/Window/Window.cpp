#include "Window.h"

std::vector<Window*> Window::instances;

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	for (Window* instance : instances) {
		instance->SetSize(width, height);
	}
}

void Window::Init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

Window::Window() : width(0), height(0), title("") {}
bool Window::CreateWindow(uint32 width, uint32 height, String title) {
	this->width = width;
	this->height = height;
	this->title = title;

	monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(monitor);

	ID = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (ID == nullptr) {
		Logger::LogLine("Window", "Failed to create Window.");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(ID);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Logger::LogLine("GLAD", "Failed to load GLAD.");
		glfwTerminate();
		return false;
	}

	instances.push_back(this);

	SetSize(width, height);
	glfwSetFramebufferSizeCallback(ID, framebuffer_size_callback);
	return true;
}
void Window::SwapBuffers() {
	glfwSwapBuffers(ID);
}

bool Window::WindowShouldClose() const {
	return glfwWindowShouldClose(ID);
}
void Window::SetWindowShouldClose(bool value) {
	glfwSetWindowShouldClose(ID, value);
}

void Window::BorderlessFullscreen() {
	glfwSetWindowMonitor(ID, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	glfwSwapInterval(1);
}
void Window::WindowedFullscreen() {
	glfwSetWindowMonitor(ID, NULL, 0, 0, width, height, 0);
	glfwMaximizeWindow(ID);
	glfwSwapInterval(0);
}
void Window::Minimize() {
	glfwIconifyWindow(ID);
}

void Window::SetSize(uint32 width, uint32 height) {
	this->height = height;
	this->width = width;
	glViewport(0, 0, width, height);
}
glm::vec2 Window::GetSize() const {
	return glm::vec2(width, height);
}
String Window::GetTitle() const {
	return title;
}
void Window::SetTitle(String title) {
	this->title = title;
	glfwSetWindowTitle(ID, title.c_str());
}

void Window::Delete() {
	glfwTerminate();
}