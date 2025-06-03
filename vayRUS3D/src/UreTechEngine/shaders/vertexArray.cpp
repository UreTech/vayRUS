#include"vertexArray.hpp"
#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>


vertexArrayObject::vertexArrayObject()
{
	// none for now
}

vertexArrayObject::~vertexArrayObject()
{
	// none for now
}

void vertexArrayObject::createObject(Vertex& vertices, unsigned int verticesCount, indexData& indices, unsigned int indicesCount)
{
	// vertex buffer
	Renderer* rnd = UreTechEngineClass::getEngine()->mainRenderer;

	VkBuffer stagingBuffer;
	VkDeviceMemory vertexBufferMemory, stagingBufferMemory;

	VkDeviceSize bufferSize = sizeof(Vertex) * verticesCount;

	rnd->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(rnd->device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, (char*)&vertices, (size_t)bufferSize);
	vkUnmapMemory(rnd->device, stagingBufferMemory);

	rnd->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	rnd->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(rnd->device, stagingBuffer, nullptr);
	vkFreeMemory(rnd->device, stagingBufferMemory, nullptr);

	// index buffer
	VkBuffer stagingIndexBuffer;
	VkDeviceMemory indexBufferMemory, stagingIndexBufferMemory;

	VkDeviceSize indexBufferSize = sizeof(indexData) * indicesCount;
	this->indexCount = indicesCount;

	rnd->createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingIndexBuffer, stagingIndexBufferMemory);

	vkMapMemory(rnd->device, stagingIndexBufferMemory, 0, indexBufferSize, 0, &data);
	memcpy(data, (char*)&indices, (size_t)indexBufferSize);
	vkUnmapMemory(rnd->device, stagingIndexBufferMemory);

	rnd->createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	rnd->copyBuffer(stagingIndexBuffer, indexBuffer, indexBufferSize);

	vkDestroyBuffer(rnd->device, stagingIndexBuffer, nullptr);
	vkFreeMemory(rnd->device, stagingIndexBufferMemory, nullptr);
}

void vertexArrayObject::bindBuffer()
{
	Renderer* rnd = UreTechEngineClass::getEngine()->mainRenderer;
	vkCmdBindPipeline(rnd->EngineCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rnd->graphicsPipeline);

	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(rnd->EngineCommandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(rnd->EngineCommandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(rnd->EngineCommandBuffer, static_cast<uint32_t>(indexCount), 1, 0, 0, 0);
}

unsigned int vertexArrayObject::getIndexCount()
{
	return indexCount;
}
