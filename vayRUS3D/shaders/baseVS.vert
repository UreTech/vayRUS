#version 450 core

 layout (location = 0) in vec3 inPosition;
 layout (location = 1) in vec2 inTexCoord;
 layout (location = 2) in vec3 inNormDat;
 layout (location = 3) in highp int inTexLvl;
 
 layout(set = 0, binding = 0) uniform UBO {
	uniform mat4 uMtxModel;
	uniform mat4 uMtxProj;
	uniform mat4 uMtxCam;
	uniform vec3 uPosCam;
 };

 
layout(location = 0) out vec2 texCoord;
layout(location = 1) out vec3 FragPos; 
layout(location = 2) out vec3 Normal;
layout(location = 3) out int vertTextureLevel;
layout(location = 4) out vec3 viewPos;

void main()
{
    gl_Position = uMtxProj * (uMtxCam * (uMtxModel * vec4(inPosition, 1.0)));
    texCoord = inTexCoord;
	Normal = mat3(transpose(inverse(uMtxModel))) * inNormDat;
	vertTextureLevel = inTexLvl;
	FragPos = inPosition;
	viewPos = uPosCam;
}