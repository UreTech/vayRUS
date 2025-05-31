#ifndef vertexArray_hpp
#define vertexArray_hpp

#include"vertexStructs.hpp"
#include <vector>
class vertexArrayObject {
public:
	vertexArrayObject();
	~vertexArrayObject();
	void createObject(Vertex& vertices,unsigned int verticesCount, indexData& indices, unsigned int indicesCount);
	void activateBuffer();
	void deactivateBuffer();
	unsigned int getIndexCount();
private:
	unsigned int VaoId, VboId, VibId,indexCount;
	std::vector<unsigned int> t_Attirbs;
};


#endif 
