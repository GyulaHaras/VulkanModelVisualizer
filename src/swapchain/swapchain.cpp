#include "swapchain.h"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface){
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if(formatCount != 0){
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if(formatCount != 0){
        details.formats.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats){
    for(const auto& availableFormat : formats){
        if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            
            return availableFormat;
        }
    }
    return formats[0]; // default and fallback value
}

VkPresentModeKHR Swapchain::choosePresentMode(const std::vector<VkPresentModeKHR>& modes){
    for(const auto& availableMode : modes){
        // VK_PRESENT_MODE_FIFO_KHR - always available (vsync)
        // VK_PRESENT_MODE_MAILBOX_KHR - Low latency triple buffering
        // VK_PRESENT_MODE_IMMEDIATE_KHR - no vsync
        if(availableMode == VK_PRESENT_MODE_MAILBOX_KHR){
            return availableMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window){
    // TODO: Consider > instead of !=
    if(capabilities.currentExtent.width <= UINT32_MAX){
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width
        );

        actualExtent.height = std::clamp(actualExtent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height
        );

        return actualExtent;
    }
}

void Swapchain::createImageViews(){
    imageViews.resize(images.size());

    for(size_t i=0; i < imageViews.size(); i++){
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = images[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = imageFormat;

        // TODO: Remove block if necessary.
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Necessary for for color images
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if(vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS){
            throw std::runtime_error("Failed creating Swap chain image views.");
        }
    }
}

// // For minimal declaration
// Swapchain::Swapchain():
//     swapChain(VK_NULL_HANDLE)
// {};

Swapchain::Swapchain(
    VkPhysicalDevice physDev,
    VkDevice dev,
    VkSurfaceKHR surf,
    const QueueFamilyIndices& indices,
    GLFWwindow* win 
): 
    physicalDevice(physDev),
    device(dev),
    surface(surf),
    queueIndices(indices),
    window(win)
    //swapChain(VK_NULL_HANDLE)
{}

Swapchain::~Swapchain(){
    if(swapChain != VK_FALSE){
        std::cout << "Cleaning up Swapchain related allocations." << std::endl;
        try {
            cleanup();
        } catch(std::exception& e){
            std::cerr << e.what() << std::endl;
        }
    }
}

void Swapchain::create(){
    createSwapchain();
    createImageViews();
}

void Swapchain::createSwapchain(){
    std::cout << "Creating swapchain..." << std::endl;
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = choosePresentMode(swapChainSupport.presentModes);
    extent = chooseExtent(swapChainSupport.capabilities, window);

    // TODO: Are we triple buffering here?
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if(swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount
    ){
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // TODO: We now have access to family indices thorugh the device provided through the class
    uint32_t queueFamilyIndices[] = { queueIndices.graphicsFamily, queueIndices.presentFamily};

    if(queueIndices.graphicsFamily != queueIndices.presentFamily){
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS){
        throw std::runtime_error("Failed to create Swapchain.");
    }

    VkResult swapchainImagesResult = vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    if(swapchainImagesResult != VK_SUCCESS){
        throw std::runtime_error("Failed to create Images vector.");
    }
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());

    imageFormat = surfaceFormat.format;
}

void Swapchain::cleanup(){
    // vkDeviceWaitIdle(device);

    if(swapChain != VK_FALSE){
        std::cout << "Destroying ImageViews." << std::endl;
        for(auto view : imageViews){
            vkDestroyImageView(device, view, nullptr);
        }
        imageViews.clear();
        std::cout << "Destroying Swapchain." << std::endl;
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        // vkDestroyDevice(device, nullptr);
        swapChain = VK_NULL_HANDLE;
    } else {
        std::cout << "Swapchain not found." << std::endl;
    }
}

bool Swapchain::operator!=(const bool& b){
    // TODO: Not a good validation, it fails after destructing swapchain
    return !((this->swapChain == VK_NULL_HANDLE) && b);
}