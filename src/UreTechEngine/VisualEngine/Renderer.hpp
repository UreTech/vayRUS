#ifndef RENDERER_HPP
#define RENDERER_HPP

#include<string>
#include<mutex>
#include<glm/glm.hpp>
#include<glm/gtx/matrix_transform_2d.hpp>
#include<fstream>
#include<iostream>
#include<vector>
#include<functional>
#include<map>
#include<UreTechEngine/utils/string/string.h>
#include<UreTechEngine/utils/dArray.h>
#include<UreTechEngine/VisualEngine/vertexStructs.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include<GLFW/glfw3native.h>
#include<vulkan/vulkan.h>
#include<imgui/imgui.h>
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_vulkan.h>
#include<imgui/imconfig.h>
#include<imgui/imgui_internal.h>
#include<imgui/imstb_rectpack.h>
#include<imgui/imstb_textedit.h>
#include<imgui/imstb_truetype.h>

namespace UreTechEngine
{
	class UreTechEngineClass;
}

typedef enum {
	FILTER_NEAREST,
	FILTER_LINEAR
} TextureFilterType;

struct ENGINE_DEFINE QueueFamilyIndices {
	uint32_t graphicsFamily = 0xFABCAF;
	uint32_t presentFamily = 0xFABCAF;

	bool isComplete();
};

QueueFamilyIndices ENGINE_DEFINE findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

class ENGINE_DEFINE Renderer {
public:
	ImGuiContext* ImGuiContext = nullptr; // ImGui context

	GLFWwindow* window;

	VkInstance instance;
	UreTechEngine::dArray<VkExtensionProperties> extensions;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;           // pyhsical device
	VkDevice device;                                            // actual device
	VkDebugUtilsMessengerEXT debugMessenger;                    // debug messenger
	VkQueue graphicsQueue;                                      // graphics queue
	VkSurfaceKHR surface = nullptr;                             // main surface
	VkQueue presentQueue;                                       // present queue
	VkCommandPool commandPool;                                  // command pool
	// VkCommandBuffer EngineCommandBuffer;                     // command buffer for engine (depracated)
	VkCommandBuffer multiThreadEngineCommandBuffer;             // command buffer for multi threading
	VkSwapchainKHR swapChain;                                   // swap chain
	// color
	UreTechEngine::dArray<VkFramebuffer> swapChainFramebuffers; // swap chain framebuffers
	UreTechEngine::dArray<VkImageView> swapChainImageViews;     // swap chain image views
	UreTechEngine::dArray<VkImage> swapChainImages;             // swap chain images

	// depth
	UreTechEngine::dArray<VkDeviceMemory> swapChainDepthImageMemories;
	UreTechEngine::dArray<VkImageView> swapChainDepthImageViews;     // swap chain image views
	UreTechEngine::dArray<VkImage> swapChainDepthImages;             // swap chain images

	VkFormat swapChainImageFormat;                              // swap chain image format
	VkExtent2D swapChainExtent;                                 // swap chain extent
	VkRenderPass renderPass;                                    // render pass
	// VkRenderPass multiThreadRenderPass;                         // render pass for multi threading
	VkSemaphore imageAvailableSemaphore;                        // image available semaphore
	VkSemaphore renderFinishedSemaphore;                        // render finished semaphore
	// VkSemaphore multiRenderFinishedSemaphore;                // multi threaded render finished semaphore (not used)
	VkFence inFlightFence;                                      // fence
	VkPipelineLayout pipelineLayout;                            // pipeline layout

	VkDescriptorPool imguiPool;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet UTEXdescriptorSet;
	VkDescriptorSetLayout UBOdescriptorSetLayout;
	VkDescriptorSetLayout UTEXdescriptorSetLayout;

	VkPipeline graphicsPipeline;
	UreTechEngine::dArray<VkShaderModule> VertexShaders;
	UreTechEngine::dArray<VkShaderModule> FragmentShaders;

	// submit headers
	VkSubmitInfo submitInfo = {};
	VkPresentInfoKHR presentInfo{};
	

	// for not "Kaboom!" at the next stages
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	UreTechEngine::UreTechEngineClass* engine = nullptr;

	Renderer();
	~Renderer();

	//default texture:
	texture emptyTx;

	// global uniforms for shaders (private ones set in meshs)
	baseVS_Uniforms vsUniforms;
	baseFS_Uniforms fsUniforms;

	// Vulkan
	VkSampler linearFilterSampler;
	VkSampler nearestFilterSampler;

	VkShaderModule createShaderModule(const char* moduleFileName);

	void setupShaderUniforms();

	void sendUniforms(VkDescriptorSet* sets = nullptr, size_t setCount = 0);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void createImageView(VkImage image, VkFormat format, VkImageView& imageView);

	VkSampler createSampler(TextureFilterType filterType);

	// headers for vulkan stuff
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	VkClearValue clearValues[2] = {};
	VkCommandBufferBeginInfo cmdBufferBeginInfo = {};

	VkCommandBufferInheritanceInfo multiThreadInheritanceInfo = {};
	VkCommandBufferBeginInfo multiThreadSecondaryCmdBufferBeginInfo = {};

	void BeginVulkanCommandBuffer();

	void EndVulkanCommandBuffer();

	void processCommandBuffers();

	void prepareNewFrame();

	void endFrame();

	void waitFrame();

	std::mutex immediateSubmitMutex;
	bool immediateSubmitInfoAvailable = false;
	bool immediateSubmitInfoSubmitted = false;
	bool engineLoopStarted = false;
	VkCommandBuffer immediateSubmitCommandBuffer;
	void immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& recordCommands);

	VkCommandBuffer createTemporalCmdBuffer();

	void processAndDestroyTemporalCmdBuffer(VkCommandBuffer cmdBuffer);

	// Vulkan
	void InitVulkan();

	void createEnginePipeline();

	void createImageSamplers();

	void InitImGuiVulkan();

	void destroyVulkan();

	bool framebufferResized = false;

	void recrateSwapChainIfNeeded();

private:
	uint32_t frameBufferImageIndex;

	UreTechEngine::string getShaderFromFile(const char* fileName);

	UreTechEngine::dArray<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// Vulkan intialization
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createVulkanInstance();
	void setupDebugMessenger();
	void selectRenderDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createSurface();
	void createCommandPool();
	void createCommandBuffers();
	void createRenderPass();
	void createFramebuffers();
	void createSyncObjects();

	void recrateSwapChain();
	void destroySwapChain();

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
};
#endif