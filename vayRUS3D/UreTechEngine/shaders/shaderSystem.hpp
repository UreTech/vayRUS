#ifndef SHADERSYSTEM_HPP
#define SHADERSYSTEM_HPP
#include<string>
#include<glm/glm.hpp>
#include<glad/glad.h>
#include<glm/gtx/matrix_transform_2d.hpp>
#include<fstream>
#include<iostream>
#include<map>
#include<../UreTechEngine/utils/string/string.h>
class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();
	void attachShader(const char* fileName,unsigned int shaderType);

	void link();

	void use();
	
	void addUniform(const std::string& varName);

	void setFloat(const std::string& varName, float val);

	void setBool(const std::string& varName, bool val);

	void setInt(const std::string& varName, int val);

	void setVec3(const std::string& varName, glm::vec3 val);

	void setVec4(const std::string& varName, glm::vec4& val);

	void setMat3(const std::string& varName, glm::mat3* val);

	void setMat4(const std::string& varName, glm::mat4* val);

	//texture
	void setTexture(const std::string& varName, int _textureLvl);
private:
	unsigned int programID;
	std::map<std::string, unsigned int> m_UniformVars;
	UreTechEngine::string getShaderFromFile(const char* fileName);

};
#endif