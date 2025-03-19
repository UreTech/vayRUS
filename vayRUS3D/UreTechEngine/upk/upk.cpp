#include "upk.h"

//upk standalone API

using namespace UreTechEngine;

std::string extractRootFromLine(const std::string& line) {
	const std::string prefix = "-root=";
	if (line.find(prefix) == 0) {
		return line.substr(prefix.length());
	}
	return "";
}

std::string remove_suffix(const std::string& input, char delimiter) {
	std::size_t pos = input.find_last_of(delimiter);
	if (pos != std::string::npos) {
		return input.substr(0, pos);
	}
	return input;
}

void parseFilePath(const std::string& filePath, std::string& fileName, std::string& P_param, std::string& L_param, std::string& S_param) {
	size_t P_pos = filePath.find("-P");
	size_t L_pos = filePath.find("-L");
	size_t S_pos = filePath.find("-S");

	fileName = filePath.substr(0, P_pos);

	if (P_pos != std::string::npos) {
		size_t end = std::min(filePath.length(), std::min(L_pos, S_pos));
		P_param = filePath.substr(P_pos + 2, end - P_pos - 2);
	}

	if (L_pos != std::string::npos) {
		size_t end = (S_pos != std::string::npos) ? S_pos : filePath.length();
		L_param = filePath.substr(L_pos + 2, end - L_pos - 2);
	}

	if (S_pos != std::string::npos) {
		S_param = filePath.substr(S_pos + 2);
	}
}

void upk_API::readAndCreateTree(std::string path)
{
	//vars
	std::vector<std::string> lines;
	std::string line;
	std::string headerLine;

	//set file access name
	fileName = remove_suffix(path, '-');

	//read encrypt or decrypt file
	UreTechEngine::Buffer hFileBuff = fileSys::readFile(path);
	if (hFileBuff.pointer == nullptr) {
		EngineConsole::log(upk_PackagerHeaderTitle  upk_error  "Can not read header file!", EngineConsole::t_error::ERROR_NORMAL);
		return;
	}
	headerLine = std::string((char*)hFileBuff.pointer, hFileBuff.size);

	//reading header
	if (headerLine.substr(0, 10) == "-encrypted") {
		isRootEncrypted = false;
		isEncrypted = true;
		EngineConsole::log(upk_PackagerHeaderTitle  upk_info  "File is encrypted!", EngineConsole::t_error::INFO_NORMAL);
		deletingArea = 11;
	}
	else if (headerLine.substr(0, 10) == "-decrypted") {
		isRootEncrypted = false;
		isEncrypted = false;
		deletingArea = 11;
		EngineConsole::log(upk_PackagerHeaderTitle  upk_info  "File is not encrypted!", EngineConsole::t_error::INFO_NORMAL);
	}
	else if (headerLine.substr(0, 15) == "-root-encrypted") {
		isRootEncrypted = true;
		isEncrypted = true;
		deletingArea = 16;
		EngineConsole::log(upk_PackagerHeaderTitle  upk_info  "File is root-encrypted!", EngineConsole::t_error::INFO_NORMAL);
	}
	else {
		EngineConsole::log(upk_PackagerHeaderTitle  upk_error  "unknown encryption header!", EngineConsole::t_error::ERROR_NORMAL);
	}
	headerLine.~basic_string();

	//read file to a vector
	std::vector<uint8_t> rawBuffer(hFileBuff.pointer, hFileBuff.pointer + hFileBuff.size);
	free(hFileBuff.pointer);
	UreTechEngine::Buffer decryptedBufferOut;

	//erase header
	rawBuffer.erase(rawBuffer.begin(), rawBuffer.begin() + deletingArea);
	std::string rawHeaderFile;

	//decrypt if encrypted
	if (isRootEncrypted) {
		UreTechEngine::Buffer tempRawBuf;
		tempRawBuf.pointer = rawBuffer.data();
		tempRawBuf.size = rawBuffer.size();
		EngineConsole::log(upk_PackagerHeaderTitle  upk_info  " decrypting header...", EngineConsole::t_error::INFO_NORMAL);
		decryptedBufferOut = encryptor::xorEncryptDecrypt(tempRawBuf, encryptionKey);
		lines.clear();

		std::stringstream ss(std::string(decryptedBufferOut.pointer, decryptedBufferOut.pointer + decryptedBufferOut.size));
		std::string line;
		rawHeaderFile = ss.str();
		while (std::getline(ss, line, '\n')) {
			lines.push_back(line);
		}
		lines.insert(lines.begin(), "-decryptedNow");
	}

	//bruh bruh
	rootPath = extractRootFromLine(lines.at(1));
	inPackageFile dummy;
	dummy.directory = "root";
	dummy.locationInPartition = -1;
	dummy.partition = -1;

	for (uint64_t i = 2; i < lines.size(); i++) {
		inPackageFile temporal;
		std::string filePath;
		std::string partition;
		std::string locationInPartition;
		std::string sizeInStr;
		parseFilePath(lines.at(i), filePath, partition, locationInPartition, sizeInStr);
		if (!(partition.empty() || partition.empty())) {
			temporal.directory = filePath;
			//console::print(upk_PackagerHeaderTitle + upk_info, partition);
			try {
				temporal.partition = std::stoull(partition);
				temporal.locationInPartition = std::stoull(locationInPartition);
				temporal.fileSize = std::stoull(sizeInStr);
			}
			catch (std::exception e) {

			}
			allFilesInPackage.push_back(temporal);
		}
	}
	allFilesInPackage.push_back(dummy);

	EngineConsole::log(upk_PackagerHeaderTitle upk_info "Package tree read! Total: " + string::stdStrToUStr(std::to_string(allFilesInPackage.size())) + " files found.", EngineConsole::t_error::INFO_NORMAL);
	for (uint64_t i = 0; i < allFilesInPackage.size(); i++) {
		if (allFilesInPackage.at(i).directory.size() > rootPath.size()) {
			EngineConsole::log(upk_PackagerHeaderTitle  upk_info  "FileDirectory: " + string::stdStrToUStr(allFilesInPackage.at(i).directory.substr(rootPath.size())), EngineConsole::t_error::INFO_NORMAL);
		}
		else {
			EngineConsole::log(upk_PackagerHeaderTitle  upk_info  "FileDirectory: " + string::stdStrToUStr(allFilesInPackage.at(i).directory), EngineConsole::t_error::INFO_NORMAL);
		}
	}
}

std::string removeFromBack(std::string source, std::string toRemove) {
	std::string mod = source;
	size_t pos = mod.find(toRemove);
	if (pos != std::string::npos) {
		mod.erase(pos, toRemove.length());
	}
	return mod;
}

std::string findAndReplace(std::string str, char toRemove, char toReplace) {
	for (uint64_t i = 0; i < str.size(); i++) {
		if (str[i] == toRemove) {
			str[i] = toReplace;
		}
	}
	return str;
}

Buffer upk_API::get(UreTechEngine::string path)
{
	path = findAndReplace(path.std_str(), '/', '\\');
	std::string removingPart = rootPath + "";
	bool notFound = true;
	inPackageFile item;
	for (uint64_t i = 0; i < allFilesInPackage.size(); i++) {
		if (removeFromBack(allFilesInPackage.at(i).directory, removingPart) == path) {
			notFound = false;
			item = allFilesInPackage.at(i);
			break;
		}
	}
	std::string partitionPath = fileName + "-" + std::to_string(item.partition) + ".upk";
	std::ifstream inputFile(partitionPath, std::ios::binary | std::ios::ate);

	UreTechEngine::Buffer readFileData;
	UreTechEngine::Buffer passFileData;

	if (!notFound) {
		while (1) {
			if (inputFile.is_open()) {
				if (!inputFile) {
					EngineConsole::log(upk_PackagerHeaderTitle "invalid partition file!", EngineConsole::t_error::WARN_CAN_CAUSE_ERROR);
					notFound = true;
					break;
				}
				inputFile.seekg(item.locationInPartition, std::ios::beg);
				std::streamsize readSize = item.fileSize;
				readFileData.size = readSize;
				readFileData.pointer = (uint8_t*)malloc(readFileData.size);
				inputFile.read((char*)readFileData.pointer, readSize);
			}
			else {
				notFound = true;
				EngineConsole::log(upk_PackagerHeaderTitle "invalid partition file!", EngineConsole::t_error::WARN_CAN_CAUSE_ERROR);
				break;
			}
			break;
		}
		if (isEncrypted) {
			UreTechEngine::Buffer decrypted = encryptor::xorEncryptDecrypt(readFileData, encryptionKey);
			free(readFileData.pointer);
			readFileData = decrypted;
		}

		if (item.fileSize != 0) {
			passFileData.pointer = readFileData.pointer;
			passFileData.size = readFileData.size;

		}
		else {
			EngineConsole::log(upk_PackagerHeaderTitle "0 byte file ignoring.", EngineConsole::t_error::ERROR_NORMAL);
			notFound = true;
		}
	}
	/*if (notFound) {
		passFileData.pointer = nullptr;
	}*/
	if (passFileData.pointer != nullptr) {
		EngineConsole::log(upk_PackagerHeaderTitle " File loaded: " + path, EngineConsole::t_error::INFO_NORMAL);
	}
	else {
		EngineConsole::log(upk_PackagerHeaderTitle upk_error " Can not open file: " + path, EngineConsole::t_error::ERROR_NORMAL);
	}
	return passFileData;
}

void upk_API::extractTo(std::string toExtractPath)
{
	EngineConsole::log("extractTo function is undefined on vayRUS Engine build!", EngineConsole::t_error::ERROR_NORMAL);
}

void upk_API::setEncryptionKey(UreTechEngine::string key)
{
	this->encryptionKey = key.std_str();
}

std::string upk_API::packageInfo()
{
	std::string res = "upk version: v1.7\n";
	res += "file: " + this->fileName;
	return res;
}
