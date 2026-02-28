#include"vertexArray.hpp"
#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<UreTechEngine/EngineCore.h>

vertexArrayObject::vertexArrayObject()
{
	// none for now
}

vertexArrayObject::~vertexArrayObject()
{
	// call destroy() if you want to free memory
}

void vertexArrayObject::createObject(Vertex& vertices, unsigned int verticesCount, indexData_32& indices, unsigned int indicesCount)
{
	// vertex buffer (unsigned int indexed)
	Renderer* rnd = UreTechEngineClass::getEngine()->mainRenderer;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

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
	VkDeviceMemory stagingIndexBufferMemory;

	VkDeviceSize indexBufferSize = sizeof(indexData_32) * indicesCount;
	this->indexCount = indicesCount;

	rnd->createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingIndexBuffer, stagingIndexBufferMemory);

	vkMapMemory(rnd->device, stagingIndexBufferMemory, 0, indexBufferSize, 0, &data);
	memcpy(data, (char*)&indices, (size_t)indexBufferSize);
	vkUnmapMemory(rnd->device, stagingIndexBufferMemory);

	rnd->createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	rnd->copyBuffer(stagingIndexBuffer, indexBuffer, indexBufferSize);

	vkDestroyBuffer(rnd->device, stagingIndexBuffer, nullptr);
	vkFreeMemory(rnd->device, stagingIndexBufferMemory, nullptr);

	this->indexType = VK_INDEX_TYPE_UINT32;
}

void vertexArrayObject::createObject(Vertex& vertices, unsigned int verticesCount, indexData_16& indices, unsigned int indicesCount)
{
	// vertex buffer (unsigned short indexed)
	Renderer* rnd = UreTechEngineClass::getEngine()->mainRenderer;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

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
	VkDeviceMemory stagingIndexBufferMemory;

	VkDeviceSize indexBufferSize = sizeof(indexData_16) * indicesCount;
	this->indexCount = indicesCount;

	rnd->createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingIndexBuffer, stagingIndexBufferMemory);

	vkMapMemory(rnd->device, stagingIndexBufferMemory, 0, indexBufferSize, 0, &data);
	memcpy(data, (char*)&indices, (size_t)indexBufferSize);
	vkUnmapMemory(rnd->device, stagingIndexBufferMemory);

	rnd->createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	rnd->copyBuffer(stagingIndexBuffer, indexBuffer, indexBufferSize);

	vkDestroyBuffer(rnd->device, stagingIndexBuffer, nullptr);
	vkFreeMemory(rnd->device, stagingIndexBufferMemory, nullptr);

	this->indexType = VK_INDEX_TYPE_UINT16;
}

void vertexArrayObject::bindAndDrawBuffer()
{
	UreTechEngineClass* eng = UreTechEngineClass::getEngine();

	// i dont have multiple pipelines for now
	// no need to bind that here again
	//vkCmdBindPipeline(eng->getThreadCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, eng->mainRenderer->graphicsPipeline);
	
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(eng->getThreadCommandBuffer(), 0, 1, &vertexBuffer, offsets);

	vkCmdBindIndexBuffer(eng->getThreadCommandBuffer(), indexBuffer, 0, this->indexType);

	vkCmdDrawIndexed(eng->getThreadCommandBuffer(), static_cast<uint32_t>(indexCount), 1, 0, 0, 0);
}

unsigned int vertexArrayObject::getIndexCount()
{
	return indexCount;
}

void vertexArrayObject::destroy()
{
	Renderer* rnd = UreTechEngineClass::getEngine()->mainRenderer;

	vkDestroyBuffer(rnd->device, vertexBuffer, nullptr);
	vkFreeMemory(rnd->device, vertexBufferMemory, nullptr);

	vkDestroyBuffer(rnd->device, indexBuffer, nullptr);
	vkFreeMemory(rnd->device, indexBufferMemory, nullptr);
}
