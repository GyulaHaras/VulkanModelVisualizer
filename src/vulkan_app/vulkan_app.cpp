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

const int MAX_FRAMES_IN_FLIGHT = 2;

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

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
){
	std::cerr << "\n=====Vulkan Validation=====\n";
	std::cerr << pCallbackData->pMessage << std::endl;
	std::cerr << "\n=====\n";

	return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pMessenger
){
	// This is the helper function that is being extended, pointed at runtime.
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance,
		"vkCreateDebugUtilsMessengerEXT"
	);

	if(func != nullptr){
		return func(
			instance,
			pCreateInfo,
			pAllocator,
			pMessenger
		);
	}
}

void destroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT pMessenger,
	const VkAllocationCallbacks* pAllocator
){
	// This is the helper function that is being extended, pointed at runtime.
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance,
		"vkDestroyDebugUtilsMessengerEXT"
	);

	if(func != nullptr){
		func(instance, pMessenger, pAllocator);	
	}
}

VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo(){
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	createInfo.messageType = 
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;

	return createInfo;
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

	debugMessenger = {};
	VkDebugUtilsMessengerCreateInfoEXT createMessengerInfo = populateDebugMessengerCreateInfo();

	VkInstanceCreateInfo createInstanceInfo{};
	createInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInstanceInfo.pApplicationInfo = &appInfo;
	createInstanceInfo.pNext = &createMessengerInfo;
	// TODO: Validate the existence of glfwExtensionCount and get the size of validationLayers instead of fixed layerCount 
	createInstanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInstanceInfo.ppEnabledExtensionNames = extensions.data();

	if(enableValidationLayers) {
		createInstanceInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInstanceInfo.enabledLayerCount = 0;
	}

	// instance = std::unique_ptr<VkInstance, VulkanDeleter>(new VkInstance(), VulkanDeleter(&instance, &debugMessenger));
	instance = {};
	VkResult instanceResult = vkCreateInstance(&createInstanceInfo, nullptr, &instance);

	std::cout << "Instance creation result code: " << instanceResult << std::endl;
	if(instanceResult != VK_SUCCESS){
		std::cout << "Instance creation error. " << instanceResult << std::endl;
		throw std::runtime_error("Failure creating Vulkan instance.");
	}
	
	createDebugUtilsMessengerEXT(
		instance,
		&createMessengerInfo,
		nullptr,
		&debugMessenger
	);
}

void VulkanApp::createSurface(){
	// surface = std::unique_ptr<VkSurfaceKHR, VulkanDeleter>(new VkSurfaceKHR(), VulkanDeleter(&instance, surface));
	surface = {};
	VkResult windowResult = glfwCreateWindowSurface(instance, window, nullptr, &surface);
}

void VulkanApp::pickPhysicalDevice(){
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if(deviceCount == 0){
		throw std::runtime_error("Failed to find any GPUs.");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);

	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

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
	// device = std::unique_ptr<VkDevice, VulkanDeleter>(new VkDevice(), VulkanDeleter(&device));
	device = {};
	try {
		std::cout << "Creating logical device..." << std::endl;
		deviceCreationResult = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
		std::cout << "Logical device status: " << deviceCreationResult << std::endl;
	} catch(VkResult& e){
		std::cout << "Failed logical device creation. Error: " << e << std::endl;
	}
	// TODO: Logical Device creation is making the program hang.
	if(deviceCreationResult != VK_SUCCESS){
		throw std::runtime_error("Failed creating logical device.");
	}

	vkGetDeviceQueue(device, 0, 0, &graphicsQueue);
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
		
		VkResult physicalDeviceSurfaceSupportStatus = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
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

void VulkanApp::createRenderPass(){
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchain->getImageFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkResult createRenderPassResult = vkCreateRenderPass(
		device,
		&renderPassInfo,
		nullptr,
		&renderPass
	);

	if(createRenderPassResult != VK_SUCCESS){
		throw std::runtime_error("Failed to create a render pass.");
	}
}

namespace fs = std::filesystem;
// TODO: Temporary loading function. Consider refactor toward a class, global definitions of shader names and a runtime glsl to spir-v compiler
std::vector<char> VulkanApp::loadSVShaders(const std::string &filename){
	// TODO: Files can load in binary format now. Create
	fs::path shader_dir_path = {SHADERS_DIR};
	// std::string vertex_shader = "triangle.vert.spv";
	fs::path full_path_dir = shader_dir_path / filename;
	std::ifstream file(full_path_dir, std::ios::ate | std::ios::binary);

	if(!file.is_open()){
		throw std::runtime_error("Failed to open file.");
	}
	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer; // TODO: Consider returning both buffers or calling the function twice receiving name of shader as an argument (future-proofing for dynamic shader selection).
}

VkShaderModule VulkanApp::createShaderModule(const std::vector<char>& shaderCode){
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

	VkShaderModule shaderModule;
	if(vkCreateShaderModule(
		device,
		&createInfo,
		nullptr,
		&shaderModule
	) != VK_SUCCESS){
		throw std::runtime_error("Failed to create shader module.");
	}
	return shaderModule;
}

std::array<VkPipelineShaderStageCreateInfo, 2> VulkanApp::createShaderStages(std::string shaderFilename){
	// Vertex and Fragment shader stages info
	std::vector<char> vertex_shader_buffer = loadSVShaders(shaderFilename + ".vert.spv");
	VkShaderModule vertex_module = createShaderModule(vertex_shader_buffer);
	std::vector<char> fragment_shader_buffer = loadSVShaders(shaderFilename + ".frag.spv");
	VkShaderModule fragment_module = createShaderModule(fragment_shader_buffer);
	
	VkPipelineShaderStageCreateInfo vertStage{};
	vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStage.module = vertex_module;	// TODO: Load vertex spir-v shader
	vertStage.pName = "main";

	VkPipelineShaderStageCreateInfo fragStage{};
	fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStage.module = fragment_module;	// TODO: Load fragment spir-v shader
	fragStage.pName = "main";

	// Wrapping stages in an array for compatibility with the graphics pipeline creation function.
	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {
		vertStage,
		fragStage
	};

	return shaderStages;
}


VkPipelineVertexInputStateCreateInfo VulkanApp::createVertexInputStateInfo(VkVertexInputBindingDescription& bindingDescription, std::array<VkVertexInputAttributeDescription, 3>& attributeDescriptions){
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	// VkVertexInputBindingDescription bindingDescription = bindingDescription;
	// std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = attributeDescriptions;

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	return vertexInputInfo;
}

VkPipelineViewportStateCreateInfo VulkanApp::createViewportStateInfo(VkViewport& viewport, VkRect2D& scissor){
	// Viewport State
	// VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchain->getExtent().width;
	viewport.height = (float)swapchain->getExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapchain->getExtent();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	return viewportState;
}

void VulkanApp::createDescriptorSetLayout(){
	// UBO Layout
	descriptorSetLayout = {};
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;
	
	VkResult createDescriptorSetLayoutResult = vkCreateDescriptorSetLayout(
		device, 
		&layoutInfo, 
		nullptr, 
		&descriptorSetLayout
	);

	if(createDescriptorSetLayoutResult != VK_SUCCESS){
		throw std::runtime_error("Failed to create descriptor set layout.");
	}
}

void VulkanApp::createGraphicsPipeline () {

	// The createShaderStages function assumes that a pair of <filename>.vert.spv and <filename>.frag.spv files exist in the shaders folder.
	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = createShaderStages("triangle");

	VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::getAttributeDescriptions();

	// Vertex Input State
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = createVertexInputStateInfo(bindingDescription, attributeDescriptions);

	// Pipeline Input Assembly state
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Viewport State and Scissor for Viewport State creation
	VkViewport viewport{};
	VkRect2D scissor{};

	VkPipelineViewportStateCreateInfo viewportState = createViewportStateInfo(viewport, scissor);
	
	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;	// clamp fragments beyond near/far planes
	rasterizer.rasterizerDiscardEnable = VK_FALSE;	// if true, geometry never passes rasterizer
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;	// fill triangles
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	
	
	// Multi Sampling
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE; // per-sample shading
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;	// no MSAA
	
	// Color Blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = 
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;	// disable blend = overwrite pixels

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	//	Pipeline Layout
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	pipelineLayout = {};
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	VkResult createPipelineLayoutResult = vkCreatePipelineLayout(
		device,
		&pipelineLayoutInfo,
		nullptr,
		&pipelineLayout
	);
	
	if(createPipelineLayoutResult != VK_SUCCESS){
		throw std::runtime_error("Failed to create a pipeline layout.");
	}

	// Render pass
	//	instance = std::unique_ptr<VkInstance, VulkanDeleter>(new VkInstance(), VulkanDeleter(&instance));
	// renderPass = std::unique_ptr<VkRenderPass, VulkanDeleter>(new VkRenderPass(), VulkanDeleter(&device, &renderPass));
	renderPass = {};
	createRenderPass();
	
	// Graphics Pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = std::size(shaderStages);
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	graphicsPipeline = {};
	VkResult createGraphicsPipelinesResult = vkCreateGraphicsPipelines(
		device,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&graphicsPipeline
	);

	if(createGraphicsPipelinesResult != VK_SUCCESS){
		throw std::runtime_error("Failed to create graphics pipeline.");
	}
}

void VulkanApp::createUniformBuffers(){
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
		createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i],
			uniformBuffersMemory[i]
		);
	}
}	

void VulkanApp::updateUniformBuffer(uint32_t currentFrame){
	UniformBufferObject ubo{};

	ubo.model = glm::mat4(1.0f);
	ubo.view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	ubo.proj = glm::perspective(
		glm::radians(45.0f),
		swapchain->getExtent().width / (float) swapchain->getExtent().height,
		0.1f,
		10.0f
	);
	ubo.proj[1][1] *= -1; // Invert Y axis for Vulkan's coordinate system

	ubo.cameraPos = glm::vec4(0.0f, 0.0f, -3.0f, 1.0f);
	ubo.lightPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	ubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	void* data;
	vkMapMemory(
		device,
		uniformBuffersMemory[currentFrame],
		0,
		sizeof(ubo),
		0,
		&data
	);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, uniformBuffersMemory[currentFrame]);
}

void VulkanApp::createDescriptorPool(){
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	VkDescriptorPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.poolSizeCount = 1;
	poolCreateInfo.pPoolSizes = &poolSize;
	poolCreateInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	VkResult createDescriptorPoolResult = vkCreateDescriptorPool(
		device,
		&poolCreateInfo,
		nullptr,
		&descriptorPool
	);
	if(createDescriptorPoolResult != VK_SUCCESS){
		throw std::runtime_error("Failed to create descriptor pool.");
	}
}

void VulkanApp::createDescriptorSets(){
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();
	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	VkResult allocateDescriptorSetsResult = vkAllocateDescriptorSets(
		device,
		&allocInfo,
		descriptorSets.data()
	);
	if(allocateDescriptorSetsResult != VK_SUCCESS){
		throw std::runtime_error("Failed to allocate descriptor sets.");			
	}
}	

void VulkanApp::updateDescriptorSets(){
	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		// Vk sctruct for updating descriptor sets. This is used to update the descriptor set with the buffer info.
		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(
			device,
			1,
			&descriptorWrite,
			0,
			nullptr
		);
	}
}

void VulkanApp::createSyncronizationObjects(){
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	// Start signaling right away to avoid waits.
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
		if(vkCreateSemaphore(
			device,
			&semaphoreInfo,
			nullptr,
			&imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(
				device,
				&semaphoreInfo,
				nullptr,
				&renderFinishedSemaphores[i]
			) != VK_SUCCESS ||
			vkCreateFence(
				device,
				&fenceInfo,
				nullptr,
				&inFlightFences[i]
			) != VK_SUCCESS
		){
			throw std::runtime_error("Failed to create sync objects.");
		}
	}

}

void VulkanApp::loadObjToVector(const std::string& filename) {
	tinyobj::ObjReader reader;
	std::string modelFileName = filename;
	fs::path modelsPath = {MODELS_DIR};
	fs::path fullModelPath = modelsPath / modelFileName;

	if(!reader.ParseFromFile(fullModelPath)){
		throw std::runtime_error("Unable to read model .obj file.");
	}

	const auto& attrib = reader.GetAttrib();
	const auto& shapes = reader.GetShapes();

	// TODO: Consider different size color vector
	tinyobj::mesh_t mesh = shapes[0].mesh;
	vertices.resize(mesh.indices.size());
	void* indices_data_ptr = mesh.indices.data();

	Vertex transformed_vertex = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};

	int32_t t_index = 0;
	for(const  tinyobj::index_t index : mesh.indices){
		transformed_vertex.pos = {
			attrib.vertices[index.vertex_index*3],
			attrib.vertices[index.vertex_index*3+1],
			attrib.vertices[index.vertex_index*3+2]
		};
		transformed_vertex.color = {
			attrib.colors[index.vertex_index*3],
			attrib.colors[index.vertex_index*3+1],
			attrib.colors[index.vertex_index*3+2]
		};
		transformed_vertex.normal = {
			attrib.normals[index.normal_index*3],
			attrib.normals[index.normal_index*3+1],
			attrib.normals[index.normal_index*3+2]
		};

		vertices[t_index++] = transformed_vertex;
	}

}

void VulkanApp::createFramebuffer(){
	swapchainFramebuffers.resize(swapchain->getImageViews().size());

	for(size_t i = 0; i < swapchain->getImageViews().size(); i++){
		VkImageView attachments[] = {
			swapchain->getImageViews()[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapchain->getExtent().width;
		framebufferInfo.height = swapchain->getExtent().height;
		framebufferInfo.layers = 1;

		if(vkCreateFramebuffer(
			device,
			&framebufferInfo,
			nullptr,
			&swapchainFramebuffers[i]
		) != VK_SUCCESS){
			throw std::runtime_error("Failed to create framebuffer.");
		}
	}
}

void VulkanApp::createCommandBuffers(){
	commandPool = {};
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily;
	VkResult commandPoolCreationResult = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
	if(commandPoolCreationResult != VK_SUCCESS){
		throw std::runtime_error("Failed to create Command Pool.");
	}
	
	commandBuffers.resize(swapchainFramebuffers.size());

	VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
	cmdBufferAllocInfo.commandPool = commandPool;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferAllocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if(vkAllocateCommandBuffers(
		device,
		&cmdBufferAllocInfo,
		commandBuffers.data()
	) != VK_SUCCESS){
		throw std::runtime_error("Failed to allocate command buffers.");
	}
}

void VulkanApp::transformVertices(){
	float scalar = 0.9f;
	glm::vec3 scale_vector = glm::vec3(1.0f, 1.0f, 1.0f)*scalar;

	// std::transform(
	// 	vertices.begin(),
	// 	vertices.end(),
	// 	vertices.begin(),
	// 	[scalar](Vertex& v){
	// 		v = v * scalar;
	// 		return v;
	// 	}
	// );

	std::transform(
		vertices.begin(),
		vertices.end(),
		vertices.begin(),
		[scale_vector](Vertex& v){
			v = v * scale_vector;
			return v;
		}
	);
}

uint32_t VulkanApp::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++){
		if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties){
			return i;
		}
	}
	throw std::runtime_error("Failed to find suitable memory type.");
}

void VulkanApp::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory){
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.allocationSize = size;
	memAllocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	vkAllocateMemory(device, &memAllocInfo, nullptr, &bufferMemory);
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void VulkanApp::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex){
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0,0};
	renderPassInfo.renderArea.extent = swapchain->getExtent();

	VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(
		commandBuffer,
		&renderPassInfo,
		VK_SUBPASS_CONTENTS_INLINE
	);
	vkCmdBindPipeline(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		graphicsPipeline
	);
	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offset[] = {0};

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offset);

	// TODO: Consider using an index buffer for better performance and memory usage.

	// Uniform Buffer Object (UBO) binding
	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0,
		1,
		&descriptorSets[currentFrame],
		0,
		nullptr
	);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);
	vkEndCommandBuffer(commandBuffer);
}

void VulkanApp::drawFrame(){
	vkWaitForFences(
		device,
		1,
		&inFlightFences[currentFrame],
		VK_TRUE,
		UINT64_MAX
	);

	updateUniformBuffer(currentFrame);

	uint32_t imageIndex;

	VkResult acquireNextImageResult =  vkAcquireNextImageKHR(
		device,
		swapchain->get(),
		UINT64_MAX,
		imageAvailableSemaphores[currentFrame],
		VK_NULL_HANDLE,
		&imageIndex
	);

	vkResetFences(
		device,
		1,
		&inFlightFences[currentFrame]
	);

	vkResetCommandBuffer(
		commandBuffers[currentFrame],
		0
	);

	recordCommandBuffer(
		commandBuffers[currentFrame],
		imageIndex
	);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = {
		imageAvailableSemaphores[currentFrame]
	};
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
	VkSemaphore signalSemaphores[] = {
		renderFinishedSemaphores[currentFrame]
	};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkResult queueSubmitResult =  vkQueueSubmit(
		graphicsQueue,
		1,
		&submitInfo,
		inFlightFences[currentFrame]
	);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapchains[] = { swapchain->get() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	VkResult queuePresentResult =  vkQueuePresentKHR(
		graphicsQueue,
		&presentInfo
	);
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanApp::cleanup(){
	// TODO: Consider moving the wait for GPU instruction here
	std::cout << "VulkanApp instance cleanup starting.." << std::endl;
	for(VkFramebuffer buffer : swapchainFramebuffers){
		vkDestroyFramebuffer(device, buffer, nullptr);
	}
	vkFreeCommandBuffers(device, commandPool, commandBuffers.size(), commandBuffers.data());
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	// TODO: Destroy pipeline layouts
	vkDestroyRenderPass(device, renderPass, nullptr);
	// TODO: Destroy Samplers once implemented
	// TODO: Destroy descriptor pools
	// TODO: Destroy descriptor set layouts

	// Semaphores may be of different sizes, so we iterate them separately
	for(VkSemaphore semaphore : renderFinishedSemaphores){
		vkDestroySemaphore(device, semaphore, nullptr);
	}
	for(VkSemaphore semaphore : imageAvailableSemaphores){
		vkDestroySemaphore(device, semaphore, nullptr);
	}
	vkDestroyCommandPool(device, commandPool, nullptr);
	// The Swapchain destructor calls the destruction funcionts for its SwapchainKHR and ImageViews
	if(swapchain) swapchain.reset();
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	// Deferred vkDestroyDebugUtilsMessengerEXT
	destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	vkDestroyInstance(instance, nullptr);
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

	swapchain.emplace(
		physicalDevice,
		device,
		surface,
		indices,
		window
	);
	if(!swapchain){
		throw std::runtime_error("Failed to initialize Swapchain container.");
	}
	swapchain->create();

	createDescriptorSetLayout();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	updateDescriptorSets();

	createGraphicsPipeline();

	createFramebuffer();
	createCommandBuffers();

	// Sync Objects
	createSyncronizationObjects();

	loadObjToVector("suzanne.obj");

	// TODO: Needs to be updated to transform normals as well. Consider refactoring the function to accept a transformation matrix and apply it to both vertices and normals.
	// transformVertices();

	// Vertex Buffer
	vertexBuffer = {};
	VkDeviceMemory vertexBufferMemory;
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		vertexBuffer, vertexBufferMemory
	);

	void* data;

	vkMapMemory(
		device,
		vertexBufferMemory,
		0,
		bufferSize,
		0,
		&data
	);

	memcpy(data, vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(device, vertexBufferMemory);
}

VulkanApp::~VulkanApp(){
	vkDeviceWaitIdle(device);
	// TODO: Destroying functions are executed on smart pointer custom deleters. Consider deletion of the cleanup function if it is no needed for other Vulkan objects
	cleanup();
}