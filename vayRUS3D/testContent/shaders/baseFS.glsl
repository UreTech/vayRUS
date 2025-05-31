#version 330 core
out vec4 fragColor;

in vec2 texCoord;
in vec3 FragPos;  
in vec3 Normal;

in vec3 viewPos;

flat in int vertTextureLevel;

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
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;

 void main()
{
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);  

        float diff = max(dot(norm, lightDir), 0.0);

        float brightness = 0.2;
        vec4 brightnessLvl = (brightness+diff) * uLightColor;

        switch (vertTextureLevel) {
            case 0:
                if(litRender0){
                    fragColor = texture2D(texture0,texCoord)*brightnessLvl;
                    break;
                }else{
                    fragColor = texture2D(texture0,texCoord);
                    break;
                }
            case 1:
                if(litRender1){
                    fragColor = texture2D(texture1,texCoord)*brightnessLvl;
                    break;
                }else{
                    fragColor = texture2D(texture1,texCoord);
                    break;
                }
            case 2:
                if(litRender2){
                    fragColor = texture2D(texture2,texCoord)*brightnessLvl;
                    break;
                }else{
                    fragColor = texture2D(texture2,texCoord);
                    break;
                }
            case 3:
                if(litRender3){
                    fragColor = texture2D(texture3,texCoord)*brightnessLvl;
                    break;
                }else{
                    fragColor = texture2D(texture3,texCoord);
                    break;
                }
            case 4:
                if(litRender4){
                    fragColor = texture2D(texture4,texCoord)*brightnessLvl;
                    break;
                }else{
                    fragColor = texture2D(texture4,texCoord);
                    break;
                }
            case 5:
                if(litRender5){
                    fragColor = texture2D(texture5,texCoord)*brightnessLvl;
                    break;
                }else{
                    fragColor = texture2D(texture5,texCoord);
                    break;
                }
        }
}