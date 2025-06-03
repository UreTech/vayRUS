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

VkShaderModule Renderer::createShaderModule(const char* moduleFileName)
{
	Buffer code = UreTechEngine::UreTechEngineClass::getEngine()->package->get(moduleFileName);
	if (code.pointer == nullptr) {
		EngineConsole::log("Invalid shader module file!", EngineConsole::WARN_CAN_CAUSE_ERROR);
	}
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.pointer);

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		EngineConsole::log("An error occurred while creating shader module!", EngineConsole::WARN_CAN_CAUSE_ERROR);
	}

	return shaderModule;
}

void Renderer::setupShaderUniforms()
{	
	VkBuffer vtxUniformBuf;
	VkBuffer frgUniformBuf;

	emptyTx = TextureManager::getInstance()->loadTextureFromFile("/engine/res/NULL_TEXTURE.png", true);

	VkDescriptorSetLayoutBinding vtxUboLayoutBinding{};
	vtxUboLayoutBinding.binding = 0;
	vtxUboLayoutBinding.descriptorCount = 1;
	vtxUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vtxUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding frgUboLayoutBinding{};
	frgUboLayoutBinding.binding = 1;
	frgUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	frgUboLayoutBinding.descriptorCount = 1;
	frgUboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	frgUboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding frgTexUboLayoutBinding{};
	frgTexUboLayoutBinding.binding = 0;
	frgTexUboLayoutBinding.descriptorCount = 16; // 16 textures max
	frgTexUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	frgTexUboLayoutBinding.pImmutableSamplers = nullptr;
	frgTexUboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding bindings[3] = { vtxUboLayoutBinding, frgUboLayoutBinding };

	VkDescriptorSetLayoutCreateInfo UBOlayoutInfo{};
	UBOlayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	UBOlayoutInfo.bindingCount = 2;
	UBOlayoutInfo.pBindings = bindings;

	vkCreateDescriptorSetLayout(device, &UBOlayoutInfo, nullptr, &UBOdescriptorSetLayout);

	VkDescriptorSetLayoutCreateInfo UTEXlayoutInfo{};
	UTEXlayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	UTEXlayoutInfo.bindingCount = 1;
	UTEXlayoutInfo.pBindings = &frgTexUboLayoutBinding;

	vkCreateDescriptorSetLayout(device, &UTEXlayoutInfo, nullptr, &UTEXdescriptorSetLayout);

	VkDescriptorSetLayout layouts[] = { UBOdescriptorSetLayout, UTEXdescriptorSetLayout };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 2;
	pipelineLayoutInfo.pSetLayouts = layouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		EngineConsole::log("Can not create PipelineLayout!", EngineConsole::ERROR_FATAL);
	}

	VkDescriptorSetAllocateInfo UTEXallocInfo{};
	UTEXallocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	UTEXallocInfo.descriptorPool = descriptorPool;
	UTEXallocInfo.descriptorSetCount = 1;
	UTEXallocInfo.pSetLayouts = &UTEXdescriptorSetLayout;

	if (vkAllocateDescriptorSets(device, &UTEXallocInfo, &UTEXdescriptorSet) != VK_SUCCESS) {
		EngineConsole::log("Can not allocate UTEXdescriptor set!", EngineConsole::ERROR_FATAL);
	}

	/* moved to mesh class
	VkDescriptorBufferInfo vtxBufferInfo{};
	vtxBufferInfo.buffer = vtxUniformBuf;
	vtxBufferInfo.offset = 0;
	vtxBufferInfo.range = sizeof(baseVS_Uniforms);
	VkWriteDescriptorSet vtxDescriptorWrite{};
	vtxDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vtxDescriptorWrite.dstSet = UBOdescriptorSet;
	vtxDescriptorWrite.dstBinding = 0;
	vtxDescriptorWrite.dstArrayElement = 0;
	vtxDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vtxDescriptorWrite.descriptorCount = 1;
	vtxDescriptorWrite.pBufferInfo = &vtxBufferInfo;

	VkDescriptorBufferInfo frgBufferInfo{};
	frgBufferInfo.buffer = frgUniformBuf;
	frgBufferInfo.offset = 0;
	frgBufferInfo.range = sizeof(baseFS_Uniforms);
	VkWriteDescriptorSet frgDescriptorWrite{};
	frgDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	frgDescriptorWrite.dstSet = UBOdescriptorSet;
	frgDescriptorWrite.dstBinding = 1;
	frgDescriptorWrite.dstArrayElement = 0;
	frgDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	frgDescriptorWrite.descriptorCount = 1;
	frgDescriptorWrite.pBufferInfo = &frgBufferInfo;

	VkDescriptorImageInfo frgTextureInfo{};
	frgTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	frgTextureInfo.imageView = emptyTx.imageView;
	frgTextureInfo.sampler = emptyTx.imageSampler;

	VkWriteDescriptorSet frgTexDescriptorWrite{};
	frgTexDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	frgTexDescriptorWrite.dstSet = UTEXdescriptorSet;
	frgTexDescriptorWrite.dstBinding = 0;
	frgTexDescriptorWrite.dstArrayElement = 0;
	frgTexDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	frgTexDescriptorWrite.descriptorCount = 1;
	frgTexDescriptorWrite.pImageInfo = &frgTextureInfo;

	VkWriteDescriptorSet descriptorWrites[3] = { vtxDescriptorWrite, frgDescriptorWrite, frgTexDescriptorWrite };

	vkUpdateDescriptorSets(device, 3, descriptorWrites, 0, nullptr);
	*/
	//immediateSubmit([&](VkCommandBuffer cmd) {
		//vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		//});
}

void Renderer::sendUniforms(VkDescriptorSet* sets, size_t setCount)
{
	VkDescriptorSet* totalSets = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet) * setCount);
	memcpy(totalSets, sets, sizeof(VkDescriptorSet) * setCount);
	vkCmdBindDescriptorSets(EngineCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, setCount, totalSets, 0, nullptr);
	free(totalSets);
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

Renderer* _renderer = nullptr;
void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	_renderer->framebufferResized = true;
}

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	EngineConsole::log("Vulkan internal ERROR 0x01", EngineConsole::LOG_ERROR);
}

void Renderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		EngineConsole::log("Vulkan internal buffer ERROR 0x02", EngineConsole::LOG_ERROR);
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(
		physicalDevice,
		memRequirements.memoryTypeBits,
		properties
	);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		EngineConsole::log("Vulkan internal buffer allocation ERROR 0x03", EngineConsole::LOG_ERROR);
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Renderer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	immediateSubmit([&](VkCommandBuffer cmd) {
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(cmd, srcBuffer, dstBuffer, 1, &copyRegion);
		});

}

void Renderer::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = format;
	imageCreateInfo.tiling = tiling;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

	if (vkCreateImage(device, &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
		EngineConsole::log("Failed to create image!", EngineConsole::WARN_CAN_CAUSE_ERROR);
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice ,memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		EngineConsole::log("Failed to allocate memory for image!", EngineConsole::WARN_CAN_CAUSE_ERROR);
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

void Renderer::createImageView(VkImage image, VkFormat format, VkImageView& imageView)
{
	VkImageViewCreateInfo viewCreateInfo = {};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = image;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = format;
	viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &viewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
		EngineConsole::log("Failed to create image view!", EngineConsole::WARN_CAN_CAUSE_ERROR);
	}
}

VkSampler Renderer::createSampler(TextureFilterType filterType)
{
	VkSampler sampler;

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	switch (filterType)
	{
	case FILTER_NEAREST:
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case FILTER_LINEAR:
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	default:
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	}

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1.0f;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
		EngineConsole::log("Failed to create texture sampler!", EngineConsole::WARN_CAN_CAUSE_ERROR);
	}

	return sampler;
}

void Renderer::BeginVulkanCommandBuffer(bool beginRenderPass)
{
	vkResetCommandBuffer(EngineCommandBuffer, 0);
	vkBeginCommandBuffer(EngineCommandBuffer, &beginInfo);
	//EngineConsole::log("command buffer started!", EngineConsole::DEBUG);

	if (beginRenderPass) {
		vkCmdBeginRenderPass(EngineCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(EngineCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}

	int dispW, dispH;
	glfwGetFramebufferSize(window, &dispW, &dispH);
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)dispW;
	viewport.height = (float)dispH;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	vkCmdSetViewport(EngineCommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(EngineCommandBuffer, 0, 1, &scissor);
}

void Renderer::EndVulkanCommandBuffer(bool endRenderPass)
{
	if (endRenderPass) {
		vkCmdEndRenderPass(EngineCommandBuffer);
	}
	vkEndCommandBuffer(EngineCommandBuffer);
	//EngineConsole::log("command buffer ended!", EngineConsole::DEBUG);
}

void Renderer::processCommandBuffers()
{
	//submit
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	submitInfo.pWaitSemaphores = waitSemaphores;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.pCommandBuffers = &EngineCommandBuffer;
	submitInfo.commandBufferCount = 1;

	submitInfo.pSignalSemaphores = &renderFinishedSemaphore;
	submitInfo.signalSemaphoreCount = 1;

	// present
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
	presentInfo.swapchainCount = 1;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence);
}

void Renderer::prepareNewFrame()
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

	vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &frameBufferImageIndex);
	renderPassBeginInfo.framebuffer = swapChainFramebuffers[frameBufferImageIndex];
	renderPassBeginInfo.renderArea.extent = capabilities.currentExtent;
}

void Renderer::endFrame()
{
	presentInfo.pSwapchains = &swapChain;
	presentInfo.pImageIndices = &frameBufferImageIndex;

	/*
	//submit
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	submitInfo.pWaitSemaphores = waitSemaphores;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = waitStages;

	VkCommandBuffer commandBuffers[] = { EngineCommandBuffer, ImGuiCommandBuffer };
	submitInfo.pCommandBuffers = commandBuffers;
	submitInfo.commandBufferCount = 2;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.pSignalSemaphores = signalSemaphores;
	submitInfo.signalSemaphoreCount = 1;

	// present
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	*/
}

void Renderer::waitFrame()
{
	vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &inFlightFence);
}

void Renderer::immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& recordCommands)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	recordCommands(commandBuffer);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	VkFence fence;
	vkCreateFence(device, &fenceInfo, nullptr, &fence);

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence);

	vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);

	vkDestroyFence(device, fence, nullptr);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void Renderer::InitVulkan()
{
	// pre set
	_renderer = this;
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	createVulkanInstance();
	EngineConsole::log("Vulkan instance created!", EngineConsole::DEBUG);
	setupDebugMessenger();
	EngineConsole::log("Vulkan debug messenger created!", EngineConsole::DEBUG);
	selectRenderDevice();
	EngineConsole::log("Vulkan render device created!", EngineConsole::DEBUG);
	createLogicalDevice();
	EngineConsole::log("Vulkan logical device created!", EngineConsole::DEBUG);
	createSurface();
	EngineConsole::log("Vulkan surface created!", EngineConsole::DEBUG);
	createSwapChain();
	EngineConsole::log("Vulkan swapChain created!", EngineConsole::DEBUG);
	createImageViews();
	EngineConsole::log("Vulkan ImageViews created!", EngineConsole::DEBUG);
	createCommandPool();
	EngineConsole::log("Vulkan command pools created!", EngineConsole::DEBUG);
	createCommandBuffers();
	EngineConsole::log("Vulkan command buffers created!", EngineConsole::DEBUG);
	createRenderPass();
	EngineConsole::log("Vulkan render passes created!", EngineConsole::DEBUG);
	createFramebuffers();
	EngineConsole::log("Vulkan framebuffers created!", EngineConsole::DEBUG);
	createSyncObjects();
	EngineConsole::log("Vulkan syncObjects created!", EngineConsole::DEBUG);

	VkDescriptorPoolSize poolSizes[] = {
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16000  },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 5000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 5000 },
	};

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = sizeof(poolSizes) / sizeof(poolSizes[0]);
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = 1000;
	
	if (vkCreateDescriptorPool(device, &poolInfo, 0, &descriptorPool) != VK_SUCCESS) {
		EngineConsole::log("Failed to create descriptor pool!", UreTechEngine::EngineConsole::ERROR_FATAL);
	}

	// render pass
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea.offset = { 0, 0 };

	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = {1.0f, 0 };
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;

	// command buffer
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;


	//submit
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	submitInfo.pWaitSemaphores = waitSemaphores;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.pCommandBuffers = &EngineCommandBuffer;
	submitInfo.commandBufferCount = 1;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.pSignalSemaphores = signalSemaphores;
	submitInfo.signalSemaphoreCount = 1;

	// present
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
}

void Renderer::createEnginePipeline()
{
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = VertexShaders[0];
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = FragmentShaders[0];
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;


	// vertex info
	VkVertexInputAttributeDescription attributeDescriptions[4];

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, tex);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, norm);

	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32_SINT;
	attributeDescriptions[3].offset = offsetof(Vertex, textureIndex);

	vertexInputInfo.vertexAttributeDescriptionCount = 4;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	// depth test
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkGraphicsPipelineCreateInfo pipelineInfo;
	memset(&pipelineInfo, 0, sizeof(VkGraphicsPipelineCreateInfo));
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDepthStencilState = &depthStencil;

	VkDynamicState dynamicStates[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		EngineConsole::log("Creating graphics pipeline failed!", EngineConsole::ERROR_FATAL);
	}
}

void Renderer::createImageSamplers()
{
	linearFilterSampler = createSampler(FILTER_LINEAR);
	nearestFilterSampler = createSampler(FILTER_NEAREST);
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
	//vkDestroyDescriptorPool(device, imguiPool, nullptr);
	ImGui_ImplVulkan_Shutdown();
	vkDestroyCommandPool(device, commandPool, nullptr);
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	//vkDestroyInstance(instance, nullptr);
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
	 std::vector<VkExtensionProperties> aExts;
	 uint32_t extensionCount = 0;
	 vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	 aExts.resize(extensionCount);
	 vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, aExts.data());

	 uint32_t glfwExtensionCount = 0;
	 const char** glfwExtensions;
	 glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	 std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	  
	 if (enableValidationLayers) {
		 extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		 //extensions.push_back(VK_EXT_LAYER_SETTINGS_EXTENSION_NAME);
		 //extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		 //extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		 //extensions.push_back(VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT);
	 }

	 for (uint64_t i = 0; i < extensions.size(); i++) {
		 bool notFound = true;
		 for (uint64_t j = 0; j < aExts.size(); j++) {
			 if (strcmp(aExts[j].extensionName, extensions[i]) == 0) {
				 notFound = false;
				 break;
			 }
		 }
		 if (notFound) {
			 EngineConsole::log("Extension not found! " + string(extensions[i]), EngineConsole::ERROR_FATAL);
		 }
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

	 // debug stuff
#ifdef _DEBUG
	 VkLayerSettingEXT layerSetting;
	 layerSetting.pLayerName = "VK_LAYER_KHRONOS_validation";
	 layerSetting.pSettingName = "enables";
	 layerSetting.type = VK_LAYER_SETTING_TYPE_STRING_EXT;
	 layerSetting.valueCount = 1;

	 // Make this static so layer setting reference remains valid after leaving constructor scope
	 static const char* layerEnables = "VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT";
	 layerSetting.pValues = &layerEnables;
#endif
	 // ****

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
#ifdef _DEBUG
	 createInfo.pNext = &layerSetting;
#else
	 createInfo.pNext = nullptr;
#endif
	 VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); // create instance

	 std::vector<VkExtensionProperties> aExts;
	 uint32_t extensionCount = 0;
	 vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	 aExts.resize(extensionCount);
	 vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, aExts.data());

	 EngineConsole::log("Available extensions:", UreTechEngine::EngineConsole::DEBUG);
	 for (uint64_t i = 0; i < aExts.size(); i++) {
		 EngineConsole::log(aExts[i].extensionName, UreTechEngine::EngineConsole::DEBUG);
	 }

	 if (result != VK_SUCCESS) {
		 if (result != VK_ERROR_EXTENSION_NOT_PRESENT) {
			 EngineConsole::log("Vulkan Instance ERROR!", UreTechEngine::EngineConsole::ERROR_FATAL);
		 }
		 else {
			 EngineConsole::log("Vulkan Instance ERROR! [VK_ERROR_EXTENSION_NOT_PRESENT]", UreTechEngine::EngineConsole::ERROR_FATAL);
		 }
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
	 if (enableValidationLayers) {
		 VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		 createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		 createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		 createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		 createInfo.pfnUserCallback = debugCallback;
		 createInfo.pUserData = nullptr;

		 if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			 EngineConsole::log("Failed to set up debug messenger!", EngineConsole::ERROR_FATAL);
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
		 char c_str[256];

		 std::wcstombs(c_str, deviceNameW.c_str(), sizeof(c_str));
		 EngineConsole::log("Using GPU: " + string(c_str), UreTechEngine::EngineConsole::DEBUG);
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
 }

 void Renderer::createCommandBuffers()
 {
	 VkCommandBufferAllocateInfo allocInfo{};
	 allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	 allocInfo.commandPool = commandPool;
	 allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	 allocInfo.commandBufferCount = 1;
	 /* unused
	 if (vkAllocateCommandBuffers(device, &allocInfo, &ImGuiCommandBuffer) != VK_SUCCESS) {
		 EngineConsole::log("Failed to allocate command buffer for ImGui!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }
	 */
	 if (vkAllocateCommandBuffers(device, &allocInfo, &EngineCommandBuffer) != VK_SUCCESS) {
		 EngineConsole::log("Failed to allocate command buffer for Engine!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }

	 PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT =
		 (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");

#ifdef _DEBUG
	 VkDebugUtilsObjectNameInfoEXT nameInfo = {};
	 nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	 nameInfo.objectType = VK_OBJECT_TYPE_COMMAND_BUFFER;
	 nameInfo.objectHandle = (uint64_t)EngineCommandBuffer;
	 nameInfo.pObjectName = "MainRenderCmdBuffer";
	 vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
#endif
 }

 void Renderer::createRenderPass()
 {
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

	 // depth buffer
	 VkAttachmentReference depthAttachmentRef{};
	 depthAttachmentRef.attachment = 1;
	 depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	 VkAttachmentDescription depthAttachment{};
	 depthAttachment.format = VK_FORMAT_D32_SFLOAT; //  32bit depth
	 depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	 depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	 depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	 depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	 depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	 depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	 depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	 VkSubpassDescription subpass{};
	 subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	 subpass.colorAttachmentCount = 1;
	 subpass.pColorAttachments = &colorAttachmentRef;
	 subpass.pDepthStencilAttachment = &depthAttachmentRef;

	 VkRenderPassCreateInfo renderPassInfo{};
	 renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	 VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
	 renderPassInfo.attachmentCount = 2;
	 renderPassInfo.pAttachments = attachments;

	 renderPassInfo.subpassCount = 1;
	 renderPassInfo.pSubpasses = &subpass;

	 VkSubpassDependency dependency{};
	 dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	 dependency.dstSubpass = 0;
	 dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	 dependency.srcAccessMask = 0;
	 dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	 dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	 renderPassInfo.dependencyCount = 1;
	 renderPassInfo.pDependencies = &dependency;

	 if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		 EngineConsole::log("Failed to create render pass!", UreTechEngine::EngineConsole::ERROR_FATAL);
	 }
 }

 void Renderer::createFramebuffers()
 {
	 swapChainFramebuffers.resize(swapChainImageViews.size());

	 for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		 VkImageView attachments[] = {
			 swapChainImageViews[i],
			 swapChainDepthImageViews[i]
		 };

		 VkFramebufferCreateInfo framebufferInfo{};
		 framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		 framebufferInfo.renderPass = renderPass;
		 framebufferInfo.attachmentCount = 2;
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
	 for (size_t i = 0; i < swapChainDepthImageViews.size(); ++i) {
		 vkDestroyImageView(device, swapChainDepthImageViews[i], nullptr);
	 }

	 vkDestroySwapchainKHR(device, swapChain, nullptr);
 }

 void Renderer::recrateSwapChain()
 {
	 createSwapChain();
	 createImageViews();
	 createFramebuffers();

	 renderPassBeginInfo.renderArea.extent = swapChainExtent;
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
		// if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
		 if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
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
	 //surfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
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

	 //depth
	 swapChainDepthImages.resize(imageCount);
	 swapChainDepthImageMemories.resize(imageCount);
	 swapChainDepthImageViews.resize(imageCount);

	 vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	 swapChainImageFormat = surfaceFormat.format;
	 swapChainExtent = extent;

	 for (size_t i = 0; i < imageCount; i++) {
		 // Create Image
		 VkImageCreateInfo imageInfo{};
		 imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		 imageInfo.imageType = VK_IMAGE_TYPE_2D;
		 imageInfo.extent.width = extent.width;
		 imageInfo.extent.height = extent.height;
		 imageInfo.extent.depth = 1;
		 imageInfo.mipLevels = 1;
		 imageInfo.arrayLayers = 1;
		 imageInfo.format = VK_FORMAT_D32_SFLOAT;
		 imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		 imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		 imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		 imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		 imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		 if (vkCreateImage(device, &imageInfo, nullptr, &swapChainDepthImages[i]) != VK_SUCCESS) {
			 EngineConsole::log("Can not create depth images!", EngineConsole::ERROR_FATAL);
		 }

		 // Allocate memory
		 VkMemoryRequirements memRequirements;
		 vkGetImageMemoryRequirements(device, swapChainDepthImages[i], &memRequirements);

		 VkMemoryAllocateInfo allocInfo{};
		 allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		 allocInfo.allocationSize = memRequirements.size;

		 VkPhysicalDeviceMemoryProperties memProperties;
		 vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		 uint32_t memoryTypeIndex = 0;
		 bool found = false;
		 for (uint32_t j = 0; j < memProperties.memoryTypeCount; j++) {
			 if ((memRequirements.memoryTypeBits & (1 << j)) &&
				 (memProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
				 memoryTypeIndex = j;
				 found = true;
				 break;
			 }
		 }

		 if (!found) EngineConsole::log("Can not create depth buffer memory!", EngineConsole::ERROR_FATAL);

		 allocInfo.memoryTypeIndex = memoryTypeIndex;

		 if (vkAllocateMemory(device, &allocInfo, nullptr, &swapChainDepthImageMemories[i]) != VK_SUCCESS) {
			 EngineConsole::log("Can not allocate depth buffer!", EngineConsole::ERROR_FATAL);
		 }

		 vkBindImageMemory(device, swapChainDepthImages[i], swapChainDepthImageMemories[i], 0);

		 // Create image view
		 VkImageViewCreateInfo viewInfo{};
		 viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		 viewInfo.image = swapChainDepthImages[i];
		 viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		 viewInfo.format = VK_FORMAT_D32_SFLOAT;
		 viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		 viewInfo.subresourceRange.baseMipLevel = 0;
		 viewInfo.subresourceRange.levelCount = 1;
		 viewInfo.subresourceRange.baseArrayLayer = 0;
		 viewInfo.subresourceRange.layerCount = 1;

		 if (vkCreateImageView(device, &viewInfo, nullptr, &swapChainDepthImageViews[i]) != VK_SUCCESS) {
			 EngineConsole::log("Can not create depth images views!", EngineConsole::ERROR_FATAL);
		 }
	 }
 }

 void Renderer::createImageViews()
 {
	 swapChainImageViews.resize(swapChainImages.size());
	 swapChainDepthImageViews.resize(swapChainDepthImages.size());

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

		 VkImageViewCreateInfo depthViewInfo{};
		 depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		 depthViewInfo.image = swapChainDepthImages[i];
		 depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		 depthViewInfo.format = VK_FORMAT_D32_SFLOAT;
		 depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		 depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		 depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		 depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		 depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		 depthViewInfo.subresourceRange.baseMipLevel = 0;
		 depthViewInfo.subresourceRange.levelCount = 1;
		 depthViewInfo.subresourceRange.baseArrayLayer = 0;
		 depthViewInfo.subresourceRange.layerCount = 1;

		 if (vkCreateImageView(device, &depthViewInfo, nullptr, &swapChainDepthImageViews[i]) != VK_SUCCESS) {
			 EngineConsole::log("Failed to create depth image views!", UreTechEngine::EngineConsole::ERROR_FATAL);
		 }
	 }
 }
