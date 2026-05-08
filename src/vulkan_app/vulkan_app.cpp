#include "vulkan_app.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool checkValidationLayerSupport(){
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector <VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for(const char* layerName : validationLayers){
		bool layerFound = false;
		std::cout << "Checking layers support:"
			<< layerName << std::endl 
			<< "------------------------" << std::endl;

		for(const auto& layerProperties : availableLayers){
			std::cout << layerProperties.layerName << std::endl;
			if(strcmp(layerName, layerProperties.layerName) == 0){
				std::cout << "------------------------" << std::endl
					<< "Layer supported." << std::endl;
				layerFound = true;
				break;
			}
		}
		if(!layerFound) {
			std::cout << "------------------------" << std::endl
				<< "Layer Support not found." << std::endl;
			return false;
		}
	}
	return true;
}

void VulkanApp::createInstance(){
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan 3D Molde Visualizer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	// TODO: Is this engine necessary if not existent?
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	if(checkValidationLayerSupport() == 0){
		std::cout << "No full support for validation layers" << std::endl;
	}

	// Get required extensions from GLFW
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, 
		glfwExtensions + glfwExtensionCount);

	if(enableValidationLayers){
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	std::cout << "Extensions list (GLFW + VK):" << std::endl;
	for(const auto& ext : extensions){
		std::cout << "\t" << ext << std::endl;
	}

	uint32_t extCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
	std::vector<VkExtensionProperties> availableExts(extCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, availableExts.data());

	std::cout << "Available Vk Instance extensions:" << std::endl;
	for (const auto& ext : availableExts) {
		std::cout << "\t" << ext.extensionName << std::endl;
	}

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.pNext = nullptr;
	// TODO: Validate the existence of glfwExtensionCount and get the size of validationLayers instead of fixed layerCount 
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if(enableValidationLayers) {
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	instance = std::unique_ptr<VkInstance, VulkanDeleter>(new VkInstance(), VulkanDeleter(instance.get()));
	VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, instance.get());

	std::cout << "Instance creation result code: " << instanceResult << std::endl;
	if(instanceResult != VK_SUCCESS){
		std::cout << "Instance creation error. " << instanceResult << std::endl;
		throw std::runtime_error("Failure creating Vulkan instance.");
	}
}

void VulkanApp::createSurface(){
	surface = std::unique_ptr<VkSurfaceKHR, VulkanDeleter>(new VkSurfaceKHR(), VulkanDeleter(instance.get(), surface.get()));
	VkResult windowResult = glfwCreateWindowSurface(*instance.get(), window, nullptr, surface.get());
}

void VulkanApp::pickPhysicalDevice(){
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

	if(deviceCount == 0){
		throw std::runtime_error("Failed to find any GPUs.");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);

	vkEnumeratePhysicalDevices(*instance.get(), &deviceCount, devices.data());

	std::cout << "Physical devices list:" << std::endl;
	for(const auto& dev : devices){
		std::cout << "\t" << dev << std::endl;
	}

	// TODO: Hard coded. Implement way to choose from available.
	physicalDevice = devices[0];

	if (physicalDevice == VK_NULL_HANDLE) {
    	throw std::runtime_error("Failed to select a physical device!");
	}
}

void VulkanApp::createLogicalDevice(){
	float queuePriority = 1.0f;

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	// TODO: Hard coded. Find correct family
	queueCreateInfo.queueFamilyIndex = 0;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.enabledExtensionCount = 1;
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	const VkDeviceQueueCreateInfo deviceQueueInfo = *createInfo.pQueueCreateInfos; 
	std::cout 
		<< "sType " << createInfo.sType << std::endl
		<< "pQueueCreateInfos " << createInfo.pQueueCreateInfos << std::endl
		<< "queueCreateInfoCount " << createInfo.queueCreateInfoCount << std::endl
		<< "pEnabledFeatures " << createInfo.pEnabledFeatures << std::endl
	<< std::endl;

	std::cout 
		<< "physicalDevice " << physicalDevice << std::endl
		<< "device " << device << std::endl
	<< std::endl;

	VkResult deviceCreationResult; 
	device = std::unique_ptr<VkDevice, VulkanDeleter>(new VkDevice(), VulkanDeleter(device.get()));
	try {
		std::cout << "Creating logical device..." << std::endl;
		deviceCreationResult = vkCreateDevice(physicalDevice, &createInfo, nullptr, device.get());
		std::cout << "Logical device status: " << deviceCreationResult << std::endl;
	} catch(VkResult& e){
		std::cout << "Failed logical device creation. Error: " << e << std::endl;
	}
	// TODO: Logical Device creation is making the program hang.
	if(deviceCreationResult != VK_SUCCESS){
		throw std::runtime_error("Failed creating logical device.");
	}

	vkGetDeviceQueue(*device.get(), 0, 0, &graphicsQueue);
	if(graphicsQueue != VK_NULL_HANDLE){
		std::cout << "Graphics queue found." << std::endl;
	}
}

void VulkanApp::getFamilyIndices(){
	std::cout << "Looking for Family indices..." << std::endl;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
	std::cout << "Queue Family count: " << queueFamilyCount << std::endl;
	for(const auto& queueFamily : queueFamilies){
		std::cout << "Family: " << queueFamily.queueFlags << std::endl;
	}

	for(int i = 0; i < queueFamilies.size(); i++){
		// std::cout << i << std::endl;
		if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
			// TODO: Do a validation based on the result of vkGetPhysicalDeviceQueueFamilyProperties
			// The QueueFamilyIndices struct has unsigned ints only, so compare to null instead or a Vulkan constant
			indices.graphicsFamily = i;
		}
		VkBool32 presentSupport = false;

		std::cout << "Physical device: " << physicalDevice << " Surface: " << surface << std::endl;
		
		VkResult physicalDeviceSurfaceSupportStatus = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, *surface.get(), &presentSupport);
		std::cout << "Physical Device Surface Support status: " << physicalDeviceSurfaceSupportStatus << std::endl;
		std::cout << "Family: " << queueFamilies[i].queueFlags << " presentSuport: " << presentSupport << std::endl;
		
		if(presentSupport){
			indices.presentFamily = i;
		}
		if(indices.graphicsFamily == VK_SUCCESS && indices.presentFamily == VK_SUCCESS) break;
	}

	// TODO: This validation may be unnecessary.
	if(indices.graphicsFamily != VK_SUCCESS || indices.presentFamily != VK_SUCCESS){
		std::runtime_error("Limited or inexistent support for indices.");
	} else {
		std::cout << "Family Indices support found." << std::endl;
	}
}

void VulkanApp::cleanup(){
	std::cout << "VulkanApp instance clean up starting.." << std::endl;
}

VulkanApp::VulkanApp(GLFWwindow *window)
	: window(window)
{
    	createInstance();
		createSurface();
		if(surface == VK_NULL_HANDLE){
			std::cout << "Surface is NULL before physical device selection.";
		}
		pickPhysicalDevice();
		createLogicalDevice();
		getFamilyIndices();

		swapchain = std::make_unique<Swapchain>(
			physicalDevice,
			*device.get(),
			*surface.get(),
			indices,
			window
		);
		(*swapchain.get()).create();
}

VulkanApp::~VulkanApp(){
	// TODO: Destroying functions are executed on smart pointer custom deleters. Consider deletion of the cleanup function if it is no needed for other Vulkan objects
	cleanup();
}