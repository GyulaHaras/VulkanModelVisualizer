
#include <string.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <filesystem>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <tiny_obj_loader.h>
#include "swapchain/swapchain.h"

void destroyDebugUtilsMessengerEXT(VkInstance instance,VkDebugUtilsMessengerEXT pMessenger,const VkAllocationCallbacks* pAllocator);

class VulkanApp {
    private:
        // Internal use functions
        // void destroyDebugUtilsMessengerEXT(	VkInstance instance,VkDebugUtilsMessengerEXT pMessenger,const VkAllocationCallbacks* pAllocator);
        // void destroyDebugUtilsMessengerEXT(VkInstance instance,VkDebugUtilsMessengerEXT pMessenger,const VkAllocationCallbacks* pAllocator);
        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void getFamilyIndices();
        void createRenderPass();
        static std::vector<char> loadSVShaders(const std::string &filename);
        VkShaderModule createShaderModule(const std::vector<char>& shaderCode);
        void createFramebuffer();
        VkCommandBuffer createCommandBuffers();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void cleanup();

        struct Vertex {
            glm::vec3 pos;
            glm::vec3 color;

            static VkVertexInputBindingDescription getBindingDescription(){
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                return bindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions(){
                std::array<VkVertexInputAttributeDescription, 2> attributes{};

                attributes[0].binding = 0;
                attributes[0].location = 0;
                attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributes[0].offset = offsetof(Vertex, pos);

                attributes[1].binding = 0;
                attributes[1].location = 1;
                attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributes[1].offset = offsetof(Vertex, color);

                return attributes;
            };

        };

        // struct VulkanDeleter{
        //     VkDebugUtilsMessengerEXT *debugMessenger;
        //     VkInstance *instance;
        //     VkSurfaceKHR *surface;
        //     VkDevice *device;
        //     VkCommandPool *commandPool;
        //     VkBuffer *buffer;
        //     VkRenderPass *renderPass;
        //     VkSemaphore *semaphore;
        //     VkFence *fence;
        //     VkDeviceMemory *vertexBufferMemory;

        //     VulkanDeleter(){}
        //     VulkanDeleter(VkInstance *instance, VkDebugUtilsMessengerEXT *debugMessenger) : instance(instance),  debugMessenger(debugMessenger){}
        //     VulkanDeleter(VkDevice *device) : device(device) {}
        //     VulkanDeleter(VkInstance *instance, VkSurfaceKHR *surface) : instance(instance), surface(surface) {}
        //     VulkanDeleter(VkDevice *device, VkCommandPool *commandPool) : device(device), commandPool(commandPool){}
        //     VulkanDeleter(VkDevice *device, VkBuffer *buffer, VkDeviceMemory *vertexBufferMemory) : device(device), buffer(buffer), vertexBufferMemory(vertexBufferMemory){}
        //     VulkanDeleter(VkDevice *device, VkRenderPass *renderPass) : device(device), renderPass(renderPass){}
        //     VulkanDeleter(VkDevice *device, VkSemaphore *semaphore) : device(device), semaphore(semaphore){}
        //     VulkanDeleter(VkDevice *device, VkFence *fence) : device(device), fence(fence){}

        //     void operator()(VkInstance *instance) const {
        //         destroyDebugUtilsMessengerEXT(*instance, *debugMessenger, nullptr);
        //         vkDestroyInstance(*instance, nullptr);
        //     }
        //     void operator()(VkSurfaceKHR *surface) const {
        //         vkDestroySurfaceKHR(*instance, *surface, nullptr);
        //     }
        //     void operator()(VkDevice *device) const {
        //         vkDestroyDevice(*device, nullptr);
        //     }
        //     // Swapchain destructor handles its cleanup by a default deleter
        //     void operator()(VkCommandPool *commandPool) const {
        //         vkDestroyCommandPool(*device, *commandPool, nullptr);
        //     }
        //     // CommandBuffer
        //     void operator()(VkCommandBuffer *commandBuffer){
        //         vkDestroyBuffer(*device, *buffer, nullptr);
        //         vkFreeMemory(*device, *vertexBufferMemory, nullptr);
        //     } 
        //     // Free memory
        //     // Render Pass
        //     void operator()(VkRenderPass *renderPass){
        //         vkDestroyRenderPass(*device, *renderPass, nullptr);
        //     }

        //     // Semaphore
        //     void operator()(VkSemaphore *semaphore){
        //         vkDestroySemaphore(*device, *semaphore, nullptr);
        //     }
        //     // Fence
        //     void operator()(VkFence *fence){
        //         vkDestroyFence(*device, *fence, nullptr);
        //     }
        //     // CommandPool

        // };

        GLFWwindow *window;
        VkDebugUtilsMessengerEXT debugMessenger;
        // std::unique_ptr<VkInstance, VulkanDeleter> instance;
        VkInstance instance;
        // std::unique_ptr<VkSurfaceKHR, VulkanDeleter> surface;
        VkSurfaceKHR surface;
        // std::unique_ptr<VkPhysicalDevice> physicalDevice = std::make_unique<VkPhysicalDevice>();
        VkPhysicalDevice physicalDevice;
        // std::unique_ptr<VkDevice, VulkanDeleter> device;
        VkDevice device;
        // std::unique_ptr<VkQueue> graphicsQueue = std::make_unique<VkQueue>();
        VkQueue graphicsQueue;
        QueueFamilyIndices indices;
        // std::unique_ptr<Swapchain> swapchain;
        std::optional<Swapchain> swapchain;
        std::vector<VkFramebuffer> swapchainFramebuffers;
        // std::unique_ptr<VkRenderPass, VulkanDeleter> renderPass;
        VkRenderPass renderPass;
        VkPipeline graphicsPipeline;
        // std::unique_ptr<VkPipelineLayout> pipelineLayout;
        // TODO: Move to its own class Ex. class RenderPipeline, etc...
        VkShaderModule vertShaderModule;    // TODO: Consider just a local variable
        VkShaderModule fragShaderModule;    // TODO: Consider just a local variable
        // std::unique_ptr<VkCommandPool, VulkanDeleter> commandPool;
        VkCommandPool commandPool;
        VkBuffer vertexBuffer;
        	// Vertex input info
	    std::vector<Vertex> vertices;
        std::vector<VkFence> inFlightFences;
        std::vector<VkSemaphore> imageAvailableSemaphores;
	    std::vector<VkSemaphore> renderFinishedSemaphores;
        size_t currentFrame = 0;
        std::vector<VkCommandBuffer> commandBuffers;
    public:
        VulkanApp(GLFWwindow *window);
        ~VulkanApp();

        void drawFrame();
};