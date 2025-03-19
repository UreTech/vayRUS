#include"shaderSystem.hpp"
#include<glm/mat3x3.hpp>
#include<../EngineCore.h>

ShaderProgram::ShaderProgram()
{
	programID = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(programID);
}

void ShaderProgram::link()
{
	glLinkProgram(programID);

	int isLinkd;
	char log[512];
	glGetProgramiv(programID, GL_LINK_STATUS, &isLinkd);
	if (!isLinkd) {
		glGetProgramInfoLog(programID, 512, 0, log);
		UreTechEngine::EngineConsole::log("Shader Linking Error!\n" + string(log), UreTechEngine::EngineConsole::ERROR_FATAL);
	}
}

void ShaderProgram::use()
{
	glUseProgram(programID);
}

void ShaderProgram::addUniform(const std::string& varName)
{
	m_UniformVars[varName] = glGetUniformLocation(programID, varName.c_str());
	if ((int)m_UniformVars[varName] == -1) {
		UreTechEngine::EngineConsole::log(string::stdStrToUStr(varName) + " " + string::stdStrToUStr(std::to_string(m_UniformVars[varName])) + " is missing!", UreTechEngine::EngineConsole::WARN_CAN_CAUSE_ERROR);
	}
	else {
		UreTechEngine::EngineConsole::log(string::stdStrToUStr(varName) + " " + string::stdStrToUStr(std::to_string(m_UniformVars[varName])), UreTechEngine::EngineConsole::INFO_NORMAL);
	}
}

void ShaderProgram::setFloat(const std::string& varName, float val)
{
	glUniform1f(m_UniformVars[varName], val);
}

void ShaderProgram::setBool(const std::string& varName, bool val)
{
	glUniform1i(m_UniformVars[varName], val);
}

void ShaderProgram::setInt(const std::string& varName, int val)
{
	glUniform1i(m_UniformVars[varName], val);
}

void ShaderProgram::setVec3(const std::string& varName, glm::vec3 val)
{
	glUniform3f(m_UniformVars[varName], val.x,val.y,val.z);
}

void ShaderProgram::setVec4(const std::string& varName, glm::vec4& val)
{
	glUniform4f(m_UniformVars[varName], val.x,val.y,val.z,val.w);
}

void ShaderProgram::setMat3(const std::string& varName, glm::mat3* val)
{
	glUniformMatrix3fv(m_UniformVars[varName],1,false,(GLfloat*)val);
}

void ShaderProgram::setMat4(const std::string& varName, glm::mat4* val)
{
	glUniformMatrix4fv(m_UniformVars[varName],1,false, (GLfloat*)val);
}

void ShaderProgram::setTexture(const std::string& varName, int _textureLvl)
{
	//UreTechEngine::EngineERROR::consoleError(varName + std::to_string(m_UniformVars[varName]),UreTechEngine::EngineERROR::INFO_NORMAL);
	glUniform1i(m_UniformVars[varName], _textureLvl);
}

 void ShaderProgram::attachShader(const char* fileName, unsigned int shaderType)
{
	 unsigned int shaderID = glCreateShader(shaderType);
	 UreTechEngine::string data = getShaderFromFile(fileName);
	 const char* sourceCode = data.c_str();

	 glShaderSource(shaderID, 1, &sourceCode, 0);

	 glCompileShader(shaderID);

	 int isCompl;
	 char log[512];
	 glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompl);
	 if (!isCompl) {
		 glGetShaderInfoLog(shaderID, 512, 0, log);
		 UreTechEngine::EngineConsole::log("ShaderType:" + string::stdStrToUStr(std::to_string(shaderType)) + "\n" + log, UreTechEngine::EngineConsole::ERROR_FATAL);
	 }

	 glAttachShader(programID, shaderID);

	 glDeleteShader(shaderID);
}

 UreTechEngine::string ShaderProgram::getShaderFromFile(const char* fileName)
 {
	 UreTechEngine::string data;
	 if (UPK_ENABLE_PACKAGE_SYSTEM) {
		 Buffer shaderBuf = UreTechEngine::UreTechEngineClass::getEngine()->package->get(fileName);
		 data = string((char*)shaderBuf.pointer, shaderBuf.size);
	 }
	 else {
		 std::ifstream file(fileName);
		 
		 if (file.is_open()) {
			 char readingChar;
			 while ((readingChar = file.get()) != EOF) {
				 data.push_back(readingChar);
			 }
			 file.close();
		 }
	 }
	 return data;
 }