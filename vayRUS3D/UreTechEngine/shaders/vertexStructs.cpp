#include"vertexStructs.hpp"

Vertex::Vertex(float x , float y , float z , float tX , float tY , glm::vec3 _norm)
{
		pos.x = x;
		pos.y = y;
		pos.z = z;

		tex.x = tX;
		tex.y = tY;

		norm = _norm;
}

Vertex::Vertex(glm::vec3 _pos, glm::vec2 _uv, glm::vec3 _norm)
{
	pos = _pos;
	tex = _uv;
	norm = _norm;
}

Vertex::Vertex(glm::vec3 _pos, glm::vec2 _uv, glm::vec3 _norm, int texInd)
{
	pos = _pos;
	tex = _uv;
	norm = _norm;
	textureIndex = texInd;
}
