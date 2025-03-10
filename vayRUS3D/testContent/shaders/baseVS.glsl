#version 330 core

 layout (location = 0) in vec3 inPosition;
 layout (location = 1) in vec2 inTexCoord;
 layout (location = 2) in vec3 inNormDat;
 layout (location = 3) in highp int inTexLvl;

 uniform mat4 uMtxModel;

 uniform mat4 uMtxProj;
 uniform mat4 uMtxCam;
 uniform vec3 uPosCam;

 out vec2 texCoord;
 out vec3 FragPos; 
 out vec3 Normal;
 out int vertTextureLevel;
 out vec3 viewPos;

void main()
{
    gl_Position = uMtxProj * (uMtxCam * (uMtxModel * vec4(inPosition, 1.0)));
    texCoord = inTexCoord;
	Normal = mat3(transpose(inverse(uMtxModel))) * inNormDat;
	vertTextureLevel = inTexLvl;
	FragPos = inPosition;
	viewPos = uPosCam;
}