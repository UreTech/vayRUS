#pragma once
#include"includes.h"
class encryptor {
public:
    static UreTechEngine::Buffer xorEncryptDecrypt(UreTechEngine::Buffer input, const std::string& key);
};