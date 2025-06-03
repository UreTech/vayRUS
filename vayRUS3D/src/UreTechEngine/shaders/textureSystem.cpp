#include"textureSystem.hpp"

#include<stb/stb_image.h>

#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtx/matrix_transform_2d.hpp>

#include<UreTechEngine/EngineCore.h>

TextureManager* TextureManager::c_Instance = nullptr;
TextureManager::TextureManager() {

}

TextureManager* TextureManager::getInstance()
{
	if (c_Instance != nullptr) {
		return c_Instance;
	}
	c_Instance = new TextureManager();

	c_Instance->engine = UreTechEngine::UreTechEngineClass::getEngine();
	c_Instance->shaderProg = c_Instance->engine->getShaderProgram();

	return c_Instance;
}

// util func
void transitionImageLayout(VkCommandBuffer commandBuffer,
	VkImage image,
	VkFormat format,
	VkImageLayout oldLayout,
	VkImageLayout newLayout) {
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags srcStage;
	VkPipelineStageFlags dstStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	}
	else {
		EngineConsole::log("Unsupported layout transition!", EngineConsole::WARN_CAN_CAUSE_ERROR);
	}

	Renderer* rnd = UreTechEngineClass::getEngine()->mainRenderer;
	rnd->immediateSubmit([&](VkCommandBuffer cmd) {
		vkCmdPipelineBarrier(
			cmd,
			srcStage, dstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
		});
}

void copyBufferToImage(VkCommandBuffer commandBuffer,
	VkBuffer buffer,
	VkImage image,
	uint32_t width,
	uint32_t height) {
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};
	Renderer* rnd = UreTechEngineClass::getEngine()->mainRenderer;
	rnd->immediateSubmit([&](VkCommandBuffer cmd) {
		vkCmdCopyBufferToImage(
			cmd,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
		});
}

texture TextureManager::loadTextureFromFile(UreTechEngine::string fileName, bool texAntiAlising)
{
	texture text;
	bool isFound = false;
	loadedTexture foundOne;
	for (uint64_t i = 0; i < loadedTexts.size(); i++) {
		if (loadedTexts[i].texuteLoadName == fileName) {
			isFound = true;
			foundOne = loadedTexts[i];
			break;
		}
	}
	if (isFound) {
		text = foundOne.texture;
	}
	else {
		int t_width, t_height, nrChannels;

		stbi_set_flip_vertically_on_load(1);

		unsigned char* textureAddr = NULL;
		if (UPK_ENABLE_PACKAGE_SYSTEM) {
			Buffer icoTmpBuf = engine->package->get(fileName);
			textureAddr = stbi_load_from_memory(icoTmpBuf.pointer, icoTmpBuf.size, &t_width, &t_height, &nrChannels, 4);
			if (icoTmpBuf.pointer != nullptr) {
				free(icoTmpBuf.pointer);
			}
		}
		else {
			textureAddr = stbi_load(fileName.c_str(), &t_width, &t_height, &nrChannels, 4);
		}
		Renderer* rnd = UreTechEngineClass::getEngine()->mainRenderer;
		VkDeviceSize imageSize = t_width * t_height * 4;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		rnd->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(rnd->device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, textureAddr, imageSize);
		vkUnmapMemory(rnd->device, stagingBufferMemory);

		rnd->createImage(t_width, t_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, text.image, text.imageMemory);

		transitionImageLayout(rnd->EngineCommandBuffer, text.image, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		copyBufferToImage(rnd->EngineCommandBuffer, stagingBuffer, text.image, t_width, t_height);

		transitionImageLayout(rnd->EngineCommandBuffer, text.image, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(rnd->device, stagingBuffer, nullptr);
		vkFreeMemory(rnd->device, stagingBufferMemory, nullptr);

		rnd->createImageView(text.image, VK_FORMAT_R8G8B8A8_SRGB, text.imageView);
		text.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;

		if (texAntiAlising) {
			text.imageSampler = rnd->linearFilterSampler;
		}
		else {
			text.imageSampler = rnd->nearestFilterSampler;
		}

		stbi_image_free(textureAddr);
	}
	return text;
}

void TextureManager::applyTexture(int levelInd, texture tex)
{
	//shaderProg->setSamplerUniform(levelInd, tex);
}

void TextureManager::applyMultipleTexture(std::vector<texture> a)
{

	//vkCmdBindDescriptorSets(shaderProg->EngineCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shaderProg->pipelineLayout, 0, 1, &descriptorSets[0], 0, nullptr); // model A için
}


