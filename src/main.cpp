#include <iostream>
#include <vulkan/vulkan.hpp>

extern "C" {
	#include "mock.h"
	#include "ModelTextParser.h"
}

int main() {
	std::cout << "Hola" << std::endl;
	print_objModel();
	greeting();
	VkInstance instance;
	return 0;
}