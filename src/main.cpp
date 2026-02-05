#include <iostream>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

extern "C" {
	#include "mock.h"
	#include "ModelTextParser.h"
}

int main() {
	std::cout << "Hola" << std::endl;
	print_objModel();
	greeting();
	GLFWwindow* window;
	VkInstance instance;
	return 0;
}