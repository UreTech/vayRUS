#ifndef RENDERER_HPP
#define RENDERER_HPP

#include<string>
#include<glm/glm.hpp>
#include<glm/gtx/matrix_transform_2d.hpp>
#include<fstream>
#include<iostream>
#include<vector>
#include<functional>
#include<map>
#include<../UreTechEngine/utils/string/string.h>
#include<../UreTechEngine/utils/dArray.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include<GLFW/glfw3native.h>
#include<vulkan/vulkan.h>
#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_vulkan.h>
#include<imconfig.h>
#include<imgui_internal.h>
#include<imstb_rectpack.h>
#include<imstb_textedit.h>
#include<imstb_truetype.h>

class Renderer {
public:
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
	VkCommandBuffer commandBuffer;                              // command buffer
	VkSwapchainKHR swapChain;                                   // swap chain
	UreTechEngine::dArray<VkFramebuffer> swapChainFramebuffers; // swap chain framebuffers
	UreTechEngine::dArray<VkImageView> swapChainImageViews;     // swap chain image views
	UreTechEngine::dArray<VkImage> swapChainImages;             // swap chain images
	VkFormat swapChainImageFormat;                              // swap chain image format
	VkExtent2D swapChainExtent;                                 // swap chain extent
	VkRenderPass renderPass;                                    // render pass
	VkSemaphore imageAvailableSemaphore;                        // image available semaphore
	VkSemaphore renderFinishedSemaphore;                        // render finished semaphore
	VkFence inFlightFence;                                      // fence
	VkPipelineLayout pipelineLayout;                            // pipeline layout

	VkDescriptorPool imguiPool;
	
	//idk what that is
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	Renderer();
	~Renderer();

	// OpenGL (OGL)
	void attachShader(const char* fileName,unsigned int shaderType);

	void link();

	void use();
	
	void addUniform(const std::string& varName);

	void setFloat(const std::string& varName, float val);

	void setBool(const std::string& varName, bool val);

	void setInt(const std::string& varName, int val);

	void setVec3(const std::string& varName, glm::vec3 val);

	void setVec4(const std::string& varName, glm::vec4& val);

	void setMat3(const std::string& varName, glm::mat3* val);

	void setMat4(const std::string& varName, glm::mat4* val);

	//texture
	void setTexture(const std::string& varName, int _textureLvl);

	// Vulkan
	void InitVulkan();

	void InitImGuiVulkan();

	void destroyVulkan();

	bool framebufferResized = false;

	void recrateSwapChainIfNeeded();

private:
	unsigned int programID;
	std::map<std::string, unsigned int> m_UniformVars;
	UreTechEngine::string getShaderFromFile(const char* fileName);

	const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// Vulkan intialization
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkCommandPool immediateCommandPool;
	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

	void createVulkanInstance();
	void setupDebugMessenger();
	void selectRenderDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createSurface();
	void createCommandPool();
	void createCommandBuffer();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
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