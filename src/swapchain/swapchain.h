#pragma once
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "helper/math.h"

struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
};

class Swapchain {
public:
    Swapchain();
    Swapchain(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkSurfaceKHR surface,
        const QueueFamilyIndices& indices,
        GLFWwindow* window
    );
    // TODO: Cleanup has to be called programmatically to avoid race conditions
    ~Swapchain();

    void create();
    void cleanup();

    VkSwapchainKHR get() const { return swapChain; }
    VkFormat getImageFormat() const { return imageFormat; }
    VkExtent2D getExtent() const { return extent; }

    const std::vector<VkImageView> getImageViews() const { return imageViews; }

    bool operator!=(const bool& b);

    private:
        // External dependencies
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        QueueFamilyIndices queueIndices;
        GLFWwindow* window;

        // Internal swapchain state
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        VkFormat imageFormat;
        VkExtent2D extent;

    private:
        void createSwapchain();
        void createImageViews();

        // helpers
        VkSurfaceFormatKHR chooseSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& formats );
        VkPresentModeKHR choosePresentMode( const std::vector<VkPresentModeKHR>& modes );
        VkExtent2D chooseExtent( const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
};