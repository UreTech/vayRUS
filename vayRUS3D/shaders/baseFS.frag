#version 450 core
//#extension GL_EXT_debug_printf : enable

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec2 texCoord;
layout(location = 1) in vec3 FragPos;  
layout(location = 2) in vec3 Normal;

layout(location = 4) in vec3 viewPos;

layout(location = 3) flat in int vertTextureLevel;

layout(set = 0, binding = 1) uniform UBO {
	uniform vec3 lightPos; 
	uniform vec4 uLightColor;
	uniform float specularStrength0; 
	uniform float specularStrength1; 
	uniform float specularStrength2;
	uniform float specularStrength3; 
	uniform float specularStrength4; 
	uniform float specularStrength5; 
	uniform bool litRender0;  
	uniform bool litRender1;  
	uniform bool litRender2;  
	uniform bool litRender3;  
	uniform bool litRender4;  
	uniform bool litRender5;  
};

layout(set = 1, binding = 0) uniform sampler2D textures[16];

void main()
{
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);  

        float diff = max(dot(norm, lightDir), 0.0);

        float brightness = 3.0;
		vec3 lightColorRGB = diff * uLightColor.rgb;
		vec4 brightnessLvl = vec4(lightColorRGB, uLightColor.a) * brightness;
		fragColor = texture(textures[vertTextureLevel], texCoord) * brightnessLvl;
}