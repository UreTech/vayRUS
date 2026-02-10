#ifndef UreTechPyramid_h
#define UreTechPyramid_h

#pragma once
#include"vertexStructs.hpp"
Vertex testPyramidVertices[5] = {
	Vertex(-0.2f,-0.2f,-0.2f,0.0f,1.0f),
	Vertex(0.2f,-0.2f,-0.2f,1.0f,1.0f),
	Vertex(0.2f,0.2f,-0.2f,0.0f,1.0f),
	Vertex(-0.2f,0.2f,-0.2f,1.0f,1.0f),

	Vertex(0.0f,0.0f,0.2f,0.5f,0.0f),

};

indexData testPyramidIndecies[] = {
0, 1, 2,
0, 2, 3,

0, 1, 4,
1, 2, 4,
2, 3, 4,
3, 0, 4
};
#endif 