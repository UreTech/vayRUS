#ifndef vertexArray_hpp
#define vertexArray_hpp

#include"vertexStructs.hpp"
#include <vector>
#include<vulkan/vulkan.h>
#include<UreTechEngine/EngineCore.h>

class vertexArrayObject {
public:
	vertexArrayObject();
	~vertexArrayObject();
	void createObject(Vertex& vertices,unsigned int verticesCount, indexData& indices, unsigned int indicesCount);
	void bindBuffer();
	unsigned int getIndexCount();
private:
	VkBuffer vertexBuffer, indexBuffer;
	unsigned int indexCount = 0;
};

#endif
