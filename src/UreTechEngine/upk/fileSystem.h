#pragma once
#include"includes.h"
#include <filesystem>
namespace fs = std::filesystem;
using namespace UreTechEngine;
class fileSys {
public:
	static bool isValidFile(std::string filePath);
	static UreTechEngine::Buffer readFile(const std::string& filePath);
	static size_t getFileSize(const std::string& filePath);
	static std::vector<std::string> getAllDirectories(std::string path);
	static std::vector<fs::path> getAllFiles(const fs::path& path);
	static std::string getFileName(std::string path);
	static std::string getFileExtension(std::string path);
	static std::string getFileRelativePath(std::string path);
	static std::string getFileRootPath(std::string path);
	static bool createFile(std::string path, UreTechEngine::Buffer toWrite);
};