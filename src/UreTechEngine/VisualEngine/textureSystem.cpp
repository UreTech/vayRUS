#include"textureSystem.hpp"

#ifdef GMODULE_ENGINE_DLL
#define STB_IMAGE_IMPLEMENTATION   
#endif
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

// pipeline barrier for VkImage
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {

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
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	}
	/*
	not needed
	else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR &&
		newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {

		barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		srcStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	}
	else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {

		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	}
	*/
	else {
		EngineConsole::log("Unsupported layout transition!", EngineConsole::WARN_CAN_CAUSE_ERROR);
	}

	Renderer* rnd = UreTechEngineClass::getEngine()->mainRenderer;

	rnd->immediateSubmitGQ([&srcStage, &dstStage, &barrier](VkCommandBuffer cmd) {
		vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		});
}

void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
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

	rnd->immediateSubmit([&buffer, &image, &region](VkCommandBuffer cmd) {
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

// most pasta-ish code ever wroten
// NOTE: change the texture filtering & comparing system
texture* TextureManager::loadTextureFromFile(UreTechEngine::string fileName, bool filteredTexture)
{
	bool isFound = false;
	bool isInQueue = false;
	uint32_t inQueueIndex = 0;
	loadedTexture foundOne;

	queueMutex.lock();

	// check if already in queue
	for (uint64_t i = 0; i < queue.size(); i++) {
		if (queue[i].toLoad.textureLoadName == fileName && queue[i].toLoad.filteredTexture == filteredTexture) {
			isInQueue = true;
			inQueueIndex = i;
			break;
		}
	}

	if (!isInQueue) {
		// check if already loaded
		for (uint64_t i = 0; i < loadedTexts.size(); i++) {
			if (loadedTexts[i].textureLoadName == fileName && loadedTexts[i].filteredTexture == filteredTexture) {
				isFound = true;
				foundOne = loadedTexts[i];
				break;
			}
		}

		// if not in queue and not loaded, add to queue and process it
		if (!isFound) {
			loadedTexture thisText;
			thisText.textureLoadName = fileName;
			thisText.filteredTexture = filteredTexture;
			inQueueTexture thisQueueItem;
			thisQueueItem.toLoad = thisText;
			queue.push_back(thisQueueItem);
		}
		else {
			// return found texture & bruh i forgot mutex
			queueMutex.unlock(); // unlock
			return foundOne.texture;
		}

		queueMutex.unlock(); // unlock
	}
	else {
		// wait for it to be loaded
		bool waiting = true;
		queue[inQueueIndex].waitingFlags.push_back(&waiting);

		queueMutex.unlock(); // unlock

		// wait
		while (waiting);
	}
	

	if (isFound) {
		return foundOne.texture;
	}
	else {
		int t_width, t_height, nrChannels;

		//stbi_set_flip_vertically_on_load(1);

		unsigned char* textureAddr = NULL;
		if (UPK_ENABLE_PACKAGE_SYSTEM) {
			Buffer texTmpBuf = engine->package->get(fileName);
			textureAddr = stbi_load_from_memory(texTmpBuf.pointer, texTmpBuf.size, &t_width, &t_height, &nrChannels, 4);
			if (texTmpBuf.pointer != nullptr) {
				free(texTmpBuf.pointer);
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

		// allocate new texture
		texture* text = new texture();

		void* data;
		vkMapMemory(rnd->device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, textureAddr, imageSize);
		vkUnmapMemory(rnd->device, stagingBufferMemory);

		rnd->createImage(t_width, t_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, text->image, text->imageMemory);
		
		transitionImageLayout(text->image, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		copyBufferToImage(stagingBuffer, text->image, t_width, t_height);

		transitionImageLayout(text->image, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(rnd->device, stagingBuffer, nullptr);
		vkFreeMemory(rnd->device, stagingBufferMemory, nullptr);

		rnd->createImageView(text->image, VK_FORMAT_R8G8B8A8_SRGB, text->imageView);
		text->imageFormat = VK_FORMAT_R8G8B8A8_SRGB;

		if (filteredTexture) {
			text->imageSampler = rnd->linearFilterSampler;
		}
		else {
			text->imageSampler = rnd->nearestFilterSampler;
		}

		stbi_image_free(textureAddr);

		queueMutex.lock();

		loadedTexture thisText;
		thisText.texture = text;
		thisText.filteredTexture = filteredTexture;
		thisText.textureLoadName = fileName;

		loadedTexts.push_back(thisText);

		// find and remove from queue
		for (uint64_t i = 0; i < queue.size(); i++) {
			if (queue[i].toLoad.textureLoadName == fileName && queue[i].toLoad.filteredTexture == filteredTexture) {

				// set waiting flags to false
				for (uint64_t j = 0; j < queue[i].waitingFlags.size(); j++) {
					*(queue[i].waitingFlags[j]) = false;
				}

				queue.remove(i);
				break;
			}
		}

		queueMutex.unlock();


		return text;
	}
}
