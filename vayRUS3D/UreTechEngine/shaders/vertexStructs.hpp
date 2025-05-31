#ifndef vertexStructs_hpp
#define vertexStructs_hpp
#include<glm/vec3.hpp>
#include<glm/vec2.hpp>
struct Vertex {
	glm::vec3 pos;
	glm::vec2 tex;
	glm::vec3 norm;
	int textureIndex = 0;
	Vertex(float x=0.0f, float y = 0.0f, float z = 0.0f, float tX = 0.0f, float tY = 0.0f, glm::vec3 _norm=glm::vec3(0.0f,0.0f,0.0f));
	Vertex(glm::vec3 _pos, glm::vec2 _uv, glm::vec3 _norm);
	Vertex(glm::vec3 _pos, glm::vec2 _uv, glm::vec3 _norm,int texInd=0);
};
typedef unsigned int indexData;

struct triangleFace {
	Vertex v0;
	Vertex v1;
	Vertex v2;
	uint64_t smoothGroup = 0;
};
#endif 
