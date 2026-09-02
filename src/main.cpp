#include <iostream>
#include <thread>
#include <chrono>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

// extern "C" {
// 	#include "mock.h"
// 	#include "ModelTextParser.h"
// }

#include "vulkan_app/vulkan_app.h"
#include "swapchain/swapchain.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

GLFWwindow* window;

void initWindow(){
	if(!glfwInit()) {
		exit(-1);
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(640, 480, "Vulkan 3D Model Visualizer", NULL, NULL);
}

void cleanup(){
	std::cout << "General clean up starting.." << std::endl;

	glfwDestroyWindow(window);
	glfwTerminate();
}

int main() {
	std::cout << "Hola" << std::endl;
	// print_objModel();
	// greeting();

	try {
		initWindow();

		if(!window){
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(window);

		{
			VulkanApp application(window);

			while(!glfwWindowShouldClose(window)){
				glClear(GL_COLOR_BUFFER_BIT);
				glfwSwapBuffers(window);
				glfwPollEvents();
				// glfwWaitEvents();
				// std::this_thread::sleep_for(std::chrono::milliseconds(16));
				application.drawFrame();
			}
		}

		// TODO: For enforcing clean up sequence, call destructors from the top layer (up until now the swapchain)
		// swapchain.cleanup();
		cleanup();
	} catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return 0;
}