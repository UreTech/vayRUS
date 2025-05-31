#include"vertexArray.hpp"
#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>


vertexArrayObject::vertexArrayObject()
{
	VaoId = 0;
	VboId = 0;
	VibId = 0;

}

vertexArrayObject::~vertexArrayObject()
{
	if (VaoId) {
		//glDeleteVertexArrays(1,&VaoId);
	}
	if (VboId) {
		//glDeleteBuffers(1, &VboId);
	}
	if (VibId) {
		//glDeleteBuffers(1, &VibId);
	}
}

void vertexArrayObject::createObject(Vertex& vertices, unsigned int verticesCount, indexData& indices, unsigned int indicesCount)
{
	//glGenVertexArrays(1, &VaoId);
	//glGenBuffers(1, &VboId);
	//glGenBuffers(1, &VibId);
	/*
	glBindVertexArray(VaoId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VibId);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verticesCount, &vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices)* indicesCount, &indices, GL_STATIC_DRAW);
	indexCount = indicesCount;

	size_t p_bufSize = (sizeof(float) * 8) + sizeof(int);

	//vertex bilgisi
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, p_bufSize, (void*)0);

	//texture bilgisi
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, p_bufSize, (void*)(3 * sizeof(float)));
	
	//normal bilgisi
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, p_bufSize, (void*)(5 * sizeof(float)));
	
	//texture level bilgisi
	glEnableVertexAttribArray(3);
	
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, p_bufSize, (void*)(8 * sizeof(float)));
	*/
}

void vertexArrayObject::activateBuffer()
{
	//glBindVertexArray(VaoId);
	for (auto next : t_Attirbs) {
		//glEnableVertexAttribArray(next);
	}
}

void vertexArrayObject::deactivateBuffer()
{
	for (auto next : t_Attirbs) {
		//glDisableVertexAttribArray(next);
	}
	//glBindVertexArray(0);
}

unsigned int vertexArrayObject::getIndexCount()
{
	return indexCount;
}
