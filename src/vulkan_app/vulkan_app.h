
#include <string.h>
#include <iostream>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "swapchain/swapchain.h"

class VulkanApp {
    private:
        GLFWwindow *window;
        VkInstance instance = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        QueueFamilyIndices indices;
        Swapchain swapchain;

        // Internal use functions
        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void getFamilyIndices();
        void cleanup();
    public:
        VulkanApp(GLFWwindow *window);
        ~VulkanApp();
};