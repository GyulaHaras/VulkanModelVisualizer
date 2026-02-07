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

	if(!glfwInit()) {
		return -1;
	}

	window = glfwCreateWindow(640, 480, "Hello GLFW", NULL, NULL);

	if(!window){
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	while(!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}