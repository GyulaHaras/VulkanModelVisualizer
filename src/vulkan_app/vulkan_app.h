
#include <string.h>
#include <iostream>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "swapchain/swapchain.h"

class VulkanApp {
    private:
        // Internal use functions
        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void getFamilyIndices();
        void cleanup();

        struct VulkanDeleter{
            VkInstance *instance;
            VkSurfaceKHR *surface;
            VkDevice *device;

            VulkanDeleter(){}
            VulkanDeleter(VkInstance *instance) : instance(instance) {}
            VulkanDeleter(VkDevice *device) : device(device) {}
            VulkanDeleter(VkInstance *instance, VkSurfaceKHR *surface) : instance(instance), surface(surface) {}

            void operator()(VkInstance *instance) const {
                vkDestroyInstance(*instance, nullptr);
            }
            void operator()(VkSurfaceKHR *surface) const {
                vkDestroySurfaceKHR(*instance, *surface, nullptr);
            }
            void operator()(VkDevice *device) const {
                vkDestroyDevice(*device, nullptr);
            }
            // Swapchain destructor handles its cleanup by a default deleter
        };

        GLFWwindow *window;
        std::unique_ptr<VkInstance, VulkanDeleter> instance;
        std::unique_ptr<VkSurfaceKHR, VulkanDeleter> surface;
        // std::unique_ptr<VkPhysicalDevice> physicalDevice = std::make_unique<VkPhysicalDevice>();
        VkPhysicalDevice physicalDevice;
        std::unique_ptr<VkDevice, VulkanDeleter> device;
        // std::unique_ptr<VkQueue> graphicsQueue = std::make_unique<VkQueue>();
        VkQueue graphicsQueue;
        QueueFamilyIndices indices;
        std::unique_ptr<Swapchain> swapchain;
    public:
        VulkanApp(GLFWwindow *window);
        ~VulkanApp();
};