#ifndef vertexStructs_hpp
#define vertexStructs_hpp

#include<glm/mat4x4.hpp>
#include<glm/vec4.hpp>
#include<glm/vec3.hpp>
#include<glm/vec2.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include<GLFW/glfw3native.h>
#include<vulkan/vulkan.h>

struct texture {
	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;
	VkFormat imageFormat;
	VkSampler imageSampler;
};

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

// shader uniform structs
struct baseVS_Uniforms {
	glm::mat4 uMtxModel;
	glm::mat4 uMtxProj;
	glm::mat4 uMtxCam;
	glm::vec3 uPosCam;
};

struct baseFS_Uniforms {
	alignas(16) glm::vec3 lightPos;
	float pad1 = 0.0f; // padding
	alignas(16) glm::vec4 uLightColor;
	float specularStrength0;
	float specularStrength1;
	float specularStrength2;
	float specularStrength3;
	float specularStrength4;
	float specularStrength5;
	bool litRender0;
	bool litRender1;
	bool litRender2;
	bool litRender3;
	bool litRender4;
	bool litRender5;
};
#endif 
