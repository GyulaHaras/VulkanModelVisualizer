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

// const std::vector<const char*> validationLayers = {
// 	"VK_LAYER_KHRONOS_validation"
// };

// const std::vector<const char*> deviceExtensions = {
// 	VK_KHR_SWAPCHAIN_EXTENSION_NAME
// };

// #ifdef NDEBUG
// const bool enableValidationLayers = false;
// #else
// const bool enableValidationLayers = true;
// #endif

// VkInstance instance;

// bool checkValidationLayerSupport(){
// 	uint32_t layerCount;
// 	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
// 	std::vector <VkLayerProperties> availableLayers(layerCount);
// 	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

// 	for(const char* layerName : validationLayers){
// 		bool layerFound = false;
// 		std::cout << "Checking layers support:"
// 			<< layerName << std::endl 
// 			<< "------------------------" << std::endl;

// 		for(const auto& layerProperties : availableLayers){
// 			std::cout << layerProperties.layerName << std::endl;
// 			if(strcmp(layerName, layerProperties.layerName) == 0){
// 				std::cout << "------------------------" << std::endl
// 					<< "Layer supported." << std::endl;
// 				layerFound = true;
// 				break;
// 			}
// 		}
// 		if(!layerFound) {
// 			std::cout << "------------------------" << std::endl
// 				<< "Layer Support not found." << std::endl;
// 			return false;
// 		}
// 	}
// 	return true;
// }

// VkInstance createInstance(){
// 	VkApplicationInfo appInfo{};
// 	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
// 	appInfo.pApplicationName = "Vulkan 3D Molde Visualizer";
// 	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
// 	// TODO: Is this engine necessary if not existent?
// 	appInfo.pEngineName = "No Engine";
// 	appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
// 	appInfo.apiVersion = VK_API_VERSION_1_3;

// 	if(checkValidationLayerSupport() == 0){
// 		std::cout << "No full support for validation layers" << std::endl;
// 	}

// 	// Get required extensions from GLFW
// 	uint32_t glfwExtensionCount = 0;
// 	const char** glfwExtensions;
// 	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

// 	std::vector<const char*> extensions(glfwExtensions, 
// 		glfwExtensions + glfwExtensionCount);
	
// 	// // TODO: changes to the extensions vector or the vk instance creaiton fail.
// 	// std::vector<const char*> extensions;
// 	// extensions.reserve(glfwExtensionCount + requiredExtensions.size()+1);
// 	// extensions.insert(extensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);
// 	// extensions.insert(extensions.end(), requiredExtensions.begin(), requiredExtensions.end());

// 	// TODO: Aparently this line is making the program hang by affecting surface creation.
// 	// Be sure to add all of the extensions and not overwrite the vector/list of ext names.
// 	if(enableValidationLayers){
// 		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
// 	}

// 	std::cout << "Extensions list (GLFW + VK):" << std::endl;
// 	for(const auto& ext : extensions){
// 		std::cout << "\t" << ext << std::endl;
// 	}

// 	uint32_t extCount = 0;
// 	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
// 	std::vector<VkExtensionProperties> availableExts(extCount);
// 	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, availableExts.data());

// 	std::cout << "Available Vk Instance extensions:" << std::endl;
// 	for (const auto& ext : availableExts) {
// 		std::cout << "\t" << ext.extensionName << std::endl;
// 	}

// 	VkInstanceCreateInfo createInfo{};
// 	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
// 	createInfo.pApplicationInfo = &appInfo;
// 	createInfo.pNext = nullptr;
// 	// TODO: Validate the existence of glfwExtensionCount and get the size of validationLayers instead of fixed layerCount 
// 	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
// 	createInfo.ppEnabledExtensionNames = extensions.data();

// 	if(enableValidationLayers) {
// 		createInfo.ppEnabledLayerNames = validationLayers.data();
// 	} else {
// 		createInfo.enabledLayerCount = 0;
// 	}
// 	VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, &instance);
// 	std::cout << "Instance creation result code: " << instanceResult << std::endl;
// 	if(instanceResult != VK_SUCCESS){
// 		std::cout << "Instance creation error. " << instanceResult << std::endl;
// 		throw std::runtime_error("Failure creating Vulkan instance.");
// 	}
// 	return instance;
// }

// VkSurfaceKHR surface = VK_NULL_HANDLE;

// void createSurface(){
// 	VkResult windowResult = glfwCreateWindowSurface(instance, window, nullptr, &surface);
// 	if(windowResult != VK_SUCCESS){
// 		std::cout << "Surface creation error. " << windowResult << std::endl;
// 		throw std::runtime_error("Failure creating Window Surface.");
// 	}
// }

// VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

// void pickPhysicalDevice(){
// 	uint32_t deviceCount = 0;
// 	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

// 	if(deviceCount == 0){
// 		throw std::runtime_error("Failed to find any GPUs.");
// 	}
// 	std::vector<VkPhysicalDevice> devices(deviceCount);

// 	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

// 	std::cout << "Physical devices list:" << std::endl;
// 	for(const auto& dev : devices){
// 		std::cout << "\t" << dev << std::endl;
// 	}

// 	// TODO: Hard coded. Implement way to choose from available.
// 	physicalDevice = devices[0];

// 	if (physicalDevice == VK_NULL_HANDLE) {
//     	throw std::runtime_error("Failed to select a physical device!");
// 	}
// }

// VkDevice device;
// VkQueue graphicsQueue;

// void createLogicalDevice(){
// 	float queuePriority = 1.0f;

// 	VkDeviceQueueCreateInfo queueCreateInfo{};
// 	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
// 	// TODO: Hard coded. Find correct family
// 	queueCreateInfo.queueFamilyIndex = 0;
// 	queueCreateInfo.queueCount = 1;
// 	queueCreateInfo.pQueuePriorities = &queuePriority;

// 	VkPhysicalDeviceFeatures deviceFeatures{};

// 	VkDeviceCreateInfo createInfo{};
// 	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
// 	createInfo.enabledExtensionCount = 1;
// 	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
// 	createInfo.pQueueCreateInfos = &queueCreateInfo;
// 	createInfo.queueCreateInfoCount = 1;
// 	createInfo.pEnabledFeatures = &deviceFeatures;

// 	const VkDeviceQueueCreateInfo deviceQueueInfo = *createInfo.pQueueCreateInfos; 
// 	std::cout 
// 		<< "sType " << createInfo.sType << std::endl
// 		<< "pQueueCreateInfos " << createInfo.pQueueCreateInfos << std::endl
// 		<< "queueCreateInfoCount " << createInfo.queueCreateInfoCount << std::endl
// 		<< "pEnabledFeatures " << createInfo.pEnabledFeatures << std::endl
// 	<< std::endl;

// 	std::cout 
// 		<< "physicalDevice " << physicalDevice << std::endl
// 		<< "device " << device << std::endl
// 	<< std::endl;

// 	VkResult deviceCreationResult; 
	
// 	try {
// 		std::cout << "Creating logical device..." << std::endl;
// 		deviceCreationResult = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
// 		std::cout << "Logical device status: " << deviceCreationResult << std::endl;
// 	} catch(VkResult& e){
// 		std::cout << "Failed logical device creation. Error: " << e << std::endl;
// 	}
// 	// TODO: Logical Device creation is making the program hang.
// 	if(deviceCreationResult != VK_SUCCESS){
// 		throw std::runtime_error("Failed creating logical device.");
// 	}

// 	vkGetDeviceQueue(device, 0, 0, &graphicsQueue);
// }

// QueueFamilyIndices indices; 
// void getFamilyIndices(){
// 	uint32_t queueFamilyCount = 0;
// 	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

// 	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
// 	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

// 	for(int i = 0; i < queueFamilies.size(); i++){
// 		if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
// 			// TODO: Do a validation based on the result of vkGetPhysicalDeviceQueueFamilyProperties
// 			// The QueueFamilyIndices struct has unsigned ints only, so compare to null instead or a Vulkan constant
// 			indices.graphicsFamily = i;
// 		}
// 		VkBool32 presentSupport = false;
// 		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
// 		if(presentSupport){
// 			indices.presentFamily = i;
// 		}
// 		if(indices.graphicsFamily == VK_SUCCESS && indices.presentFamily == VK_SUCCESS) break;
// 	}

// 	// TODO: This validation may be unnecessary.
// 	if(indices.graphicsFamily != VK_SUCCESS || indices.presentFamily != VK_SUCCESS){
// 		std::runtime_error("Limited or inexistent support for indices.");
// 	}
// }

// Swapchain swapchain;

void cleanup(){
	std::cout << "General clean up starting.." << std::endl;
	// vkDeviceWaitIdle(device);
	// // TODO: As additional dependencies need to be free before vk devices destruction,
	// // a procedure for streamlining the process may be necessary to avoid polluting the arguments
	// if(swapchain != VK_TRUE){
	// 	// std::cout << "Swapchain instance found. Starting clean up." << std::endl;
	// 	swapchain.cleanup();
	// }

	// // TODO: Consider destroying these inside of the swapchain destructors
	// // vkDestroyDevice(device, nullptr);
	// vkDestroySurfaceKHR(instance, surface, nullptr);
	// vkDestroyInstance(instance, nullptr);

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

		// createInstance();
		// createSurface();
		// if(surface == VK_NULL_HANDLE){
		// 	std::cout << "Surface is NULL before physical device selection.";
		// }
		// pickPhysicalDevice();
		// createLogicalDevice();
		// // TODO: Define createImageViews and findQueueFamilies in Swapchain class
		// // QueueFamilyIndices indices = findQueueFamilies()
		// getFamilyIndices();

		// Swapchain swapchain2(
		// 	physicalDevice,
		// 	device,
		// 	surface,
		// 	indices,
		// 	window
		// );

		// swapchain = swapchain2;

		// swapchain.create();

		VulkanApp application(window);

		while(!glfwWindowShouldClose(window)){
			glClear(GL_COLOR_BUFFER_BIT);
			glfwSwapBuffers(window);
			glfwPollEvents();
			// glfwWaitEvents();
			// std::this_thread::sleep_for(std::chrono::milliseconds(16));
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