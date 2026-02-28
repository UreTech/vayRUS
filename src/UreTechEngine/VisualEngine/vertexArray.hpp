#ifndef vertexArray_hpp
#define vertexArray_hpp

#include"vertexStructs.hpp"
#include <vector>
#include<vulkan/vulkan.h>

class vertexArrayObject {
public:
	vertexArrayObject();
	~vertexArrayObject();
	void createObject(Vertex& vertices,unsigned int verticesCount, indexData_32& indices, unsigned int indicesCount);
	void createObject(Vertex& vertices,unsigned int verticesCount, indexData_16& indices, unsigned int indicesCount);
	void bindAndDrawBuffer();
	unsigned int getIndexCount();
	void destroy();
private:
	VkBuffer vertexBuffer, indexBuffer;
	VkDeviceMemory vertexBufferMemory, indexBufferMemory;
	unsigned int indexCount = 0;
	VkIndexType indexType = VK_INDEX_TYPE_UINT32;
};

#endif
