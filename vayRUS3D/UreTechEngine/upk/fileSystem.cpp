#include "fileSystem.h"

namespace fs = std::filesystem;

bool fileSys::isValidFile(std::string filePath)
{
    if (fs::exists(filePath)) {
        return true;
    }
    else {
        return false;
    }
}

UreTechEngine::Buffer fileSys::readFile(const std::string& filePath) {
    UreTechEngine::Buffer result;
    uint8_t* buffer = nullptr;
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        EngineConsole::log(upk_PackagerFileSysHeaderTitle + upk_error + " Error at opening file(0x0):" + filePath, EngineConsole::t_error::ERROR_NORMAL);
        EngineConsole::log("bruh", EngineConsole::t_error::ERROR_NORMAL);
        return result;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = (uint8_t*)malloc(size);
    result.pointer = buffer;
    result.size = size;

    if (buffer == nullptr) {
        EngineConsole::log(upk_PackagerFileSysHeaderTitle + upk_error + " Bad allocation!", EngineConsole::t_error::ERROR_NORMAL);
        return result;
    }

    if (!file.read((char*)buffer, size)) {
        EngineConsole::log(upk_PackagerFileSysHeaderTitle + upk_error + " Error at reading file(0x1):" + filePath, EngineConsole::t_error::ERROR_NORMAL);
        return result;
    }

#ifdef debugFileReaderOutput
    EngineConsole::log(upk_PackagerDebugFileSysHeaderTitle + upk_info + "File read successfully:" + filePath, EngineConsole::t_error::INFO_NORMAL);
#endif // debugFileReaderOutput

    return result;
}


size_t fileSys::getFileSize(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate); // dosyanýn sonuna git
    if (file.is_open()) {
        std::streampos fileSize = file.tellg(); // tellg() ile dosyanýn boyutunu al
        file.close();
        //std::cout << filePath << " -[Size]-> " << fileSize << std::endl;
        return fileSize;
    }
    else {
        EngineConsole::log(upk_PackagerFileSysHeaderTitle + upk_error + "An error occured while reading file size!", EngineConsole::t_error::ERROR_NORMAL);
        return -1;
    }
}

std::vector<fs::path> fileSys::getAllFiles(const fs::path& path) {
    std::vector<fs::path> result;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            result.push_back(entry.path());
        }
    }
    return result;
}

std::string fileSys::getFileName(std::string path)
{
    fs::path _file(path);
    return _file.stem().string();
}

std::string fileSys::getFileExtension(std::string path)
{
    fs::path filePath(path);
    return filePath.extension().string();
}

std::string fileSys::getFileRelativePath(std::string path)
{
    fs::path _file(path);
    return _file.relative_path().string();
}

std::string fileSys::getFileRootPath(std::string path)
{
    fs::path _file(path);
    return _file.root_directory().string();
}

bool fileSys::createFile(std::string path, UreTechEngine::Buffer toWrite) {
    //createing missing directories
    std::filesystem::path pth(path);
    std::filesystem::create_directories(pth.parent_path());

    std::ofstream file(path, std::ios::binary);

    if (!file) {
        EngineConsole::log(upk_PackagerFileSysHeaderTitle + upk_error + "An error occurred while creating file! " + path, EngineConsole::t_error::ERROR_NORMAL);
        return false;
    }

    file.write((char*)toWrite.pointer, toWrite.size);

    file.close();

#ifdef debugFileWriterOutput
    EngineConsole::log(upk_PackagerDebugFileSysHeaderTitle + upk_info + "File created! Path:" + path, EngineConsole::t_error::INFO_NORMAL);
#endif // debugFileWriterOutput

    return true;
}


std::vector<std::string> fileSys::getAllDirectories(const std::string path) {
    std::vector<std::string> result;
    std::vector<std::string> stack; // Dizinleri geçici olarak saklamak için bir yýðýn kullanýyoruz

    stack.push_back(path);

    while (!stack.empty()) {
        std::string currentPath = stack.back();
        stack.pop_back();

        for (const auto& entry : fs::directory_iterator(currentPath)) {
            if (entry.is_directory()) {
                result.push_back(entry.path().string());

#ifdef debugFileSearchOutput
                EngineConsole::log(upk_PackagerDebugFileSysHeaderTitle + upk_info + " Found folder:" + entry.path().string(), EngineConsole::t_error::INFO_NORMAL);
#endif // debugFileInputOutput	

                stack.push_back(entry.path().string());
            }
        }
    }

    return result;
}
