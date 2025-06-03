#pragma once
#ifndef UreTechCube_h
#define UreTechCube_h

#include"UreTechEngine/shaders/vertexStructs.hpp"
Vertex testCubeVertices[24] = {
	Vertex(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f),
	Vertex( 0.5f, -0.5f, -0.5f, 0.0f, 1.0f),
	Vertex( 0.5f,  0.5f, -0.5f, 0.0f, 0.0f),
	Vertex(-0.5f,  0.5f, -0.5f, 1.0f, 0.0f),
	Vertex(-0.5f, -0.5f,  0.5f, 1.0f, 1.0f),
	Vertex( 0.5f, -0.5f,  0.5f, 0.0f, 1.0f),
	Vertex( 0.5f,  0.5f,  0.5f, 0.0f, 0.0f),
	Vertex(-0.5f,  0.5f,  0.5f, 1.0f, 0.0f),
	Vertex( 0.5f, -0.5f, -0.5f, 1.0f, 1.0f),
	Vertex( 0.5f,  0.5f, -0.5f, 0.0f, 1.0f),
	Vertex( 0.5f,  0.5f,  0.5f, 0.0f, 0.0f),
	Vertex( 0.5f, -0.5f,  0.5f, 1.0f, 0.0f),
	Vertex(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f),
	Vertex(-0.5f,  0.5f, -0.5f, 0.0f, 1.0f),
	Vertex(-0.5f,  0.5f,  0.5f, 0.0f, 0.0f),
	Vertex(-0.5f, -0.5f,  0.5f, 1.0f, 0.0f),
	Vertex(-0.5f,  0.5f, -0.5f, 1.0f, 1.0f),
	Vertex( 0.5f,  0.5f, -0.5f, 0.0f, 1.0f),
	Vertex( 0.5f,  0.5f,  0.5f, 0.0f, 0.0f),
	Vertex(-0.5f,  0.5f,  0.5f, 1.0f, 0.0f),
	Vertex(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f),
	Vertex( 0.5f, -0.5f, -0.5f, 0.0f, 1.0f),
	Vertex( 0.5f, -0.5f,  0.5f, 0.0f, 0.0f),
	Vertex(-0.5f, -0.5f,  0.5f, 1.0f, 0.0f)
};

indexData testCubeIndecies[] = {
	0, 1, 2,
	2, 3, 0,
	4, 5, 6,
	6, 7, 4,
	8, 9, 10,
	10, 11, 8,
	12, 13, 14,
	14, 15, 12,
	16, 17, 18,
	18, 19, 16,
	20, 21, 22,
	22, 23, 20

};
#endif 