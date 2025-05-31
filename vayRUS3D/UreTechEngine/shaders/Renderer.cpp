#include"Renderer.hpp"
#include<glm/mat3x3.hpp>
#include<../EngineCore.h>
#include <set>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include<GLFW/glfw3native.h>
#include<vulkan/vulkan.h>
#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_vulkan.h>

struct QueueFamilyIndices {
	uint32_t graphicsFamily = 0xFABCAF;
	uint32_t presentFamily = 0xFABCAF;

	bool isComplete() {
		return graphicsFamily != 0xFABCAF && presentFamily != 0xFABCAF;
	}
};

Renderer::Renderer()
{
	//programID = glCreateProgram();
}

Renderer::~Renderer()
{
	//glDeleteProgram(programID);
}

void Renderer::link()
{
	//glLinkProgram(programID);

	int isLinkd;
	char log[512];
	//glGetProgramiv(programID, GL_LINK_STATUS, &isLinkd);
	if (!isLinkd) {
		//glGetProgramInfoLog(programID, 512, 0, log);
		UreTechEngine::EngineConsole::log("Shader Linking Error!\n" + string(log), UreTechEngine::EngineConsole::ERROR_FATAL);
	}
}

void Renderer::use()
{
	//glUseProgram(programID);
}

void Renderer::addUniform(const std::string& varName)
{
	//m_UniformVars[varName] = glGetUniformLocation(programID, varName.c_str());
	if ((int)m_UniformVars[varName] == -1) {
		UreTechEngine::EngineConsole::log(string::stdStrToUStr(varName) + " " + string::stdStrToUStr(std::to_string(m_UniformVars[varName])) + " is missing!", UreTechEngine::EngineConsole::WARN_CAN_CAUSE_ERROR);
	}
	else {
		UreTechEngine::EngineConsole::log(string::stdStrToUStr(varName) + " " + string::stdStrToUStr(std::to_string(m_UniformVars[varName])), UreTechEngine::EngineConsole::INFO);
	}
}

void Renderer::setFloat(const std::string& varName, float val)
{
	//glUniform1f(m_UniformVars[varName], val);
}

void Renderer::setBool(const std::string& varName, bool val)
{
	//glUniform1i(m_UniformVars[varName], val);
}

void Renderer::setInt(const std::string& varName, int val)
{
	//glUniform1i(m_UniformVars[varName], val);
}

void Renderer::setVec3(const std::string& varName, glm::vec3 val)
{
	//glUniform3f(m_UniformVars[varName], val.x,val.y,val.z);
}

void Renderer::setVec4(const std::string& varName, glm::vec4& val)
{
	//glUniform4f(m_UniformVars[varName], val.x,val.y,val.z,val.w);
}

void Renderer::setMat3(const std::string& varName, glm::mat3* val)
{
	///glUniformMatrix3fv(m_UniformVars[varName],1,false,(GLfloat*)val);
}

void Renderer::setMat4(const std::string& varName, glm::mat4* val)
{
	///glUniformMatrix4fv(m_UniformVars[varName],1,false, (GLfloat*)val);
}

void Renderer::setTexture(const std::string& varName, int _textureLvl)
{
	//UreTechEngine::EngineERROR::consoleError(varName + std::to_string(m_UniformVars[varName]),UreTechEngine::EngineERROR::INFO_NORMAL);
	//glUniform1i(m_UniformVars[varName], _textureLvl);
}
Renderer* _rendrer = nullptr;
void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	_rendrer->framebufferResized = true;
}

void Renderer::InitVulkan()
{
	_rendrer = this;
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	createVulkanInstance();
	setupDebugMessenger();
	selectRenderDevice();
	createLogicalDevice();
	createSurface();
	createSwapChain();
	createImageViews();
	createCommandPool();
	createCommandBuffer();
	createRenderPass();
	createFramebuffers();
	createSyncObjects();
}


QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilyCount; i++) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		if (surface != VK_NULL_HANDLE) {
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		}
		else {
			indices.presentFamily = -1;
		}

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}
	}

	return indices;
}

void Renderer::InitImGuiVulkan()
{
	// ImGui baþlat
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForVulkan(window, true);

	VkDescriptorPoolSize poolSizes[] = {
	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
	{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 }
	};

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = sizeof(poolSizes) / sizeof(poolSizes[0]);
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = 1000;

	if (vkCreateDescriptorPool(device, &poolInfo, 0, &imguiPool) != VK_SUCCESS) {
		EngineConsole::log("Failed to create descriptor pool for ImGui!", UreTechEngine::EngineConsole::ERROR_FATAL);
	}

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance;
	init_info.PhysicalDevice = physicalDevice;
	init_info.Device = device;
	init_info.Queue = graphicsQueue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 2;
	init_info.ImageCount = 2;
	init_info.UseDynamicRendering = false;
	init_info.RenderPass = renderPass;

	ImGui_ImplVulkan_Init(&init_info);
	ImGui_ImplVulkan_CreateFontsTexture();
	ImGui_ImplVulkan_DestroyFontsTexture();
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, messenger, pAllocator);
	}
}

void Renderer::destroyVulkan()
{
	destroySwapChain();
	vkDestroyDescriptorPool(device, imguiPool, nullptr);
	ImGui_ImplVulkan_Shutdown();
	vkDestroyCommandPool(device, commandPool, nullptr);
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Renderer::recrateSwapChainIfNeeded()
{
	vkDeviceWaitIdle(device);
	if (framebufferResized) {
		framebufferResized = false;
		destroySwapChain();
		recrateSwapChain();
	}
}

 void Renderer::attachShader(const char* fileName, unsigned int shaderType)
{
	 /*
	 unsigned int shaderID = glCreateShader(shaderType);
	 UreTechEngine::string data = getShaderFromFile(fileName);
	 const char* sourceCode = data.c_str();

	 glShaderSource(shaderID, 1, &sourceCode, 0);

	 glCompileShader(shaderID);

	 int isCompl;
	 char log[512];
	 glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompl);
	 if (!isCompl) {
		 glGetShaderInfoLog(shaderID, 512, 0, log);
		 UreTechEngine::EngineConsole::log("ShaderType:" + string::stdStrToUStr(std::to_string(shaderType)) + "\n" + log, UreTechEngine::EngineConsole::ERROR_FATAL);
	 }

	 glAttachShader(programID, shaderID);

	 glDeleteShader(shaderID);
	 */
}

 UreTechEngine::string Renderer::getShaderFromFile(const char* fileName)
 {
	 UreTechEngine::string data;
	 if (UPK_ENABLE_PACKAGE_SYSTEM) {
		 Buffer shaderBuf = UreTechEngine::UreTechEngineClass::getEngine()->package->get(fileName);
		 data = string((char*)shaderBuf.pointer, shaderBuf.size);
	 }
	 else {
		 std::ifstream file(fileName);
		 
		 if (file.is_open()) {
			 char readingChar;
			 while ((readingChar = file.get()) != EOF) {
				 data.push_back(readingChar);
			 }
			 file.close();
		 }
	 }
	 return data;
 }

 bool Renderer::checkValidationLayerSupport()
 {
	 uint32_t layerCount;
	 vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	 std::vector<VkLayerProperties> availableLayers(layerCount);
	 vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	 for (const char* layerName : validationLayers) {
		 bool layerFound = false;

		 for (const auto& layerProperties : availableLayers) {
			 if (strcmp(layerName, layerProperties.layerName) == 0) {
				 layerFound = true;
				 break;
			 }
		 }

		 if (!layerFound) {
			 return false;
		 }
	 }

	 return true;
 }
 std::vector<const char*> Renderer::getRequiredExtensions()
 {
	 uint32_t glfwExtensionCount = 0;
	 const char** glfwExtensions;
	 glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	 std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	 if (enableValidationLayers) {
		 extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	 }

	 return extensions;
 }

 VkExtent2D Renderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
 {
	 if (capabilities.currentExtent.width != UINT32_MAX) {
		 return capabilities.currentExtent;
	 }
	 else {
		 int width, height;
		 glfwGetFramebufferSize(window, &width, &height);

		 VkExtent2D actualExtent = {
			 static_cast<uint32_t>(width),
			 static_cast<uint32_t>(height)
		 };

		 actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		 actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		 return actualExtent;
	 }
 }

 static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	 VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	 VkDebugUtilsMessageTypeFlagsEXT messageType,
	 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	 void* pUserData) {
	 EngineConsole::log("VKAPI VLAYER CALLBACK:" + string(pCallbackData->pMessage), UreTechEngine::EngineConsole::INFO);
	 return VK_FALSE;
 }

 void Renderer::createVulkanInstance()
 {

	 if (enableValidationLayers && !checkValidationLayerSupport()) {
		 EngineConsole::log("Validation layers requested, but not available!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }

	 // App Information
	 VkApplicationInfo appInfo{};
	 appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	 appInfo.pApplicationName = ENGINE_WINDOW_TITLE;
	 appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	 appInfo.pEngineName = "vayRUS3D Engine";
	 appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	 appInfo.apiVersion = VK_API_VERSION_1_0;

	 // Instance Information
	 VkInstanceCreateInfo createInfo{};
	 createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	 createInfo.pApplicationInfo = &appInfo;

	 if (enableValidationLayers) {
		 createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		 createInfo.ppEnabledLayerNames = validationLayers.data();
	 }
	 else {
		 createInfo.enabledLayerCount = 0;
	 }

	 // Extensions
	 auto extensions = getRequiredExtensions();
	 createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	 createInfo.ppEnabledExtensionNames = extensions.data();

	 VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); // create instance

	 if (result != VK_SUCCESS) {
		 EngineConsole::log("Vulkan Instance ERROR!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }
 }

 VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	 auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	 if (func != nullptr) {
		 return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	 }
	 else {
		 return VK_ERROR_EXTENSION_NOT_PRESENT;
	 }
 }

 void Renderer::setupDebugMessenger()
 {
	 // debug...
	 uint32_t extensionCount = 0;
	 vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	 extensions.resize(extensionCount);
	 vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	 EngineConsole::log("Available extensions:", UreTechEngine::EngineConsole::DEBUG);
	 for (uint64_t i = 0; i < extensions.size(); i++) {
		 EngineConsole::log(extensions[i].extensionName, UreTechEngine::EngineConsole::DEBUG);
	 }

	 if (enableValidationLayers) {
		 VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		 createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		 createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		 createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		 createInfo.pfnUserCallback = debugCallback;
		 createInfo.pUserData = nullptr;

		 if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			 throw std::runtime_error("failed to set up debug messenger!");
		 }
	 }
 }

 bool isDeviceSuitable(VkPhysicalDevice device) {
	 VkPhysicalDeviceProperties deviceProperties;
	 VkPhysicalDeviceFeatures deviceFeatures;
	 vkGetPhysicalDeviceProperties(device, &deviceProperties);
	 vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	 return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		 deviceFeatures.geometryShader;
 }

 int rateDeviceSuitability(VkPhysicalDevice device) {
	 VkPhysicalDeviceProperties deviceProperties;
	 VkPhysicalDeviceFeatures deviceFeatures;
	 vkGetPhysicalDeviceProperties(device, &deviceProperties);
	 vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	 int score = 0;
	 if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		 score += 1000;
	 }
	 score += deviceProperties.limits.maxImageDimension2D;
	 if (!deviceFeatures.geometryShader) {
		 return 0;
	 }
	 return score;
 }

 void Renderer::selectRenderDevice()
 {
	 uint32_t deviceCount = 0;
	 vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	 std::vector<VkPhysicalDevice> devices(deviceCount);
	 vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	 VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;

	 for (uint32_t i = 0; i < deviceCount; ++i) {
		 VkPhysicalDeviceProperties props;
		 vkGetPhysicalDeviceProperties(devices[i], &props);

		 std::wstring deviceNameW(props.deviceName, props.deviceName + strlen(props.deviceName));
		 std::wstring message = L"Do you want to use that GPU?\n" + deviceNameW;

		 int result = MessageBoxW(nullptr, message.c_str(), L"Using GPU", MB_YESNO | MB_ICONQUESTION);
		 if (result == IDYES) {
			 selectedDevice = devices[i];
			 break;
		 }
	 }

	 if (selectedDevice == VK_NULL_HANDLE) {
		 EngineConsole::log("Your GPU is not supporting Vulkan or no any gpu found!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }
	 else {
		 physicalDevice = selectedDevice;
	 }
 }

 void Renderer::createLogicalDevice()
 {
	 QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

	 VkDeviceQueueCreateInfo queueCreateInfo{};
	 queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	 queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	 queueCreateInfo.queueCount = 1;

	 float queuePriority = 1.0f;
	 queueCreateInfo.pQueuePriorities = &queuePriority;

	 VkPhysicalDeviceFeatures deviceFeatures{};


	 QueueFamilyIndices _indices = findQueueFamilies(physicalDevice, surface);

	 VkDeviceCreateInfo createInfo{};
	 createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	 createInfo.pEnabledFeatures = &deviceFeatures;
	 createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	 createInfo.enabledExtensionCount = deviceExtensions.size();
	 createInfo.pQueueCreateInfos = &queueCreateInfo;
	 createInfo.queueCreateInfoCount = 1;

	 createInfo.pEnabledFeatures = &deviceFeatures;
	 
	 if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		 EngineConsole::log("Failed to create logical device!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }
 }

 void Renderer::createSurface()
 {
	 VkWin32SurfaceCreateInfoKHR createInfo{};
	 createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	 createInfo.hwnd = glfwGetWin32Window(window);
	 createInfo.hinstance = GetModuleHandle(nullptr);

	 if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
		 EngineConsole::log("Failed to create window surface!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }
	 QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	 vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
	 vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);
 }

 void Renderer::createCommandPool()
 {
	 QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

	 VkCommandPoolCreateInfo poolInfo{};
	 poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	 poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	 poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	 if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		 EngineConsole::log("Failed to create command pool!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }

	 poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	 poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	 poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	 if (vkCreateCommandPool(device, &poolInfo, nullptr, &immediateCommandPool) != VK_SUCCESS) {
		 EngineConsole::log("Failed to create immediate command pool!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }
 }

 void Renderer::createCommandBuffer()
 {
	 VkCommandBufferAllocateInfo allocInfo{};
	 allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	 allocInfo.commandPool = commandPool;
	 allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	 allocInfo.commandBufferCount = 1;

	 if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		 throw std::runtime_error("failed to allocate command buffers!");
	 }
 }

 void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
 {
	 VkCommandBufferBeginInfo beginInfo{};
	 beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	 beginInfo.flags = 0;
	 beginInfo.pInheritanceInfo = nullptr;

	 if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		 throw std::runtime_error("failed to begin recording command buffer!");
	 }
 }

 void Renderer::createRenderPass()
 {
	 // Render Pass Oluþturuluyor
	 VkAttachmentDescription colorAttachment{};
	 colorAttachment.format = swapChainImageFormat;
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

	 VkRenderPassCreateInfo renderPassInfo{};
	 renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	 renderPassInfo.attachmentCount = 1;
	 renderPassInfo.pAttachments = &colorAttachment;
	 renderPassInfo.subpassCount = 1;
	 renderPassInfo.pSubpasses = &subpass;

	 if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		 EngineConsole::log("Failed to create render pass!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }

	 VkGraphicsPipelineCreateInfo pipelineInfo{};
	 pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	 pipelineInfo.renderPass = renderPass;

 }

 void Renderer::createFramebuffers()
 {
	 swapChainFramebuffers.resize(swapChainImageViews.size());

	 for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		 VkImageView attachments[] = {
			 swapChainImageViews[i]
		 };

		 VkFramebufferCreateInfo framebufferInfo{};
		 framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		 framebufferInfo.renderPass = renderPass;
		 framebufferInfo.attachmentCount = 1;
		 framebufferInfo.pAttachments = attachments;
		 framebufferInfo.width = swapChainExtent.width;
		 framebufferInfo.height = swapChainExtent.height;
		 framebufferInfo.layers = 1;

		 if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			 EngineConsole::log("Failed to create framebuffer!", UreTechEngine::EngineConsole::ERROR_FATAL);
		 }
	 }
 }

 void Renderer::createSyncObjects()
 {
	 VkSemaphoreCreateInfo semaphoreInfo{};
	 semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	 VkFenceCreateInfo fenceInfo{};
	 fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	 fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	 if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		 vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
		 vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
		 EngineConsole::log("Failed to create synchronization objects for a frame!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }
 }

 void Renderer::destroySwapChain()
 {
	 for (size_t i = 0; i < swapChainFramebuffers.size(); ++i) {
		 vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
	 }
	 for (size_t i = 0; i < swapChainImageViews.size(); ++i) {
		 vkDestroyImageView(device, swapChainImageViews[i], nullptr);
	 }
	 vkDestroySwapchainKHR(device, swapChain, nullptr);
 }

 void Renderer::recrateSwapChain()
 {
	 createSwapChain();
	 createImageViews();
	 createFramebuffers();
 }


 Renderer::SwapChainSupportDetails Renderer::querySwapChainSupport(VkPhysicalDevice device)
 {
	 SwapChainSupportDetails details;
	 vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	 uint32_t formatCount;
	 vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	 if (formatCount != 0) {
		 details.formats.resize(formatCount);
		 vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	 }
	 uint32_t presentModeCount;
	 vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	 if (presentModeCount != 0) {
		 details.presentModes.resize(presentModeCount);
		 vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	 }
	 return details;
 }

 VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	 for (const auto& availableFormat : availableFormats) {
		 if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			 return availableFormat;
		 }
	 }

	 return availableFormats[0];
 }

 VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	 for (const auto& availablePresentMode : availablePresentModes) {
		 if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			 return availablePresentMode;
		 }
	 }

	 return VK_PRESENT_MODE_FIFO_KHR;
 }

 void Renderer::createSwapChain()
 {
	 SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	 VkSurfaceFormatKHR surfaceFormat;
	 surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	 surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	 VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	 VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	 uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	 if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
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

	 QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	 uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

	 if (indices.graphicsFamily != indices.presentFamily) {
		 createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		 createInfo.queueFamilyIndexCount = 2;
		 createInfo.pQueueFamilyIndices = queueFamilyIndices;
	 }
	 else {
		 createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		 createInfo.queueFamilyIndexCount = 0;
		 createInfo.pQueueFamilyIndices = nullptr;
	 }


	 createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	 createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	 createInfo.presentMode = presentMode;
	 createInfo.clipped = VK_TRUE;

	 createInfo.oldSwapchain = VK_NULL_HANDLE;
	 if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		 EngineConsole::log("Failed to create swap chain!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }

	 vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	 swapChainImages.resize(imageCount);
	 vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	 swapChainImageFormat = surfaceFormat.format;
	 swapChainExtent = extent;

 }

 void Renderer::createImageViews()
 {
	 swapChainImageViews.resize(swapChainImages.size());

	 for (size_t i = 0; i < swapChainImages.size(); i++) {
		 VkImageViewCreateInfo createInfo{};
		 createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		 createInfo.image = swapChainImages[i];
		 createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		 createInfo.format = swapChainImageFormat;
		 createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		 createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		 createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		 createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		 createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		 createInfo.subresourceRange.baseMipLevel = 0;
		 createInfo.subresourceRange.levelCount = 1;
		 createInfo.subresourceRange.baseArrayLayer = 0;
		 createInfo.subresourceRange.layerCount = 1;

		 if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			 EngineConsole::log("Failed to create image views!", UreTechEngine::EngineConsole::ERROR_FATAL);
		 }
	 }
 }
