#pragma once
#include"includes.h"
#include <string>


using namespace UreTechEngine;

struct fileStruct {
	std::string directory = "";
	std::string absoluteDirectory = "";

	std::string fileName = "";
	std::string fileExtension = "";

	uint8_t* data = nullptr;
	size_t dataSize = 0;
};
struct inPackageFile {
	std::string directory;
	uint64_t partition;
	uint64_t locationInPartition;
	uint64_t fileSize;
};

class upk_API {
public:
	void readAndCreateTree(std::string path);
	Buffer get(UreTechEngine::string path);
	void extractTo(std::string toExtractPath);
	void setEncryptionKey(UreTechEngine::string key);
	std::string packageInfo();
private:
	std::vector<inPackageFile> allFilesInPackage;
	std::string rootPath;
	std::string fileName;
	std::string encryptionKey = "NO KEY XD";
	bool isRootEncrypted = false;
	bool isEncrypted = false;
	uint64_t deletingArea = 0;
};