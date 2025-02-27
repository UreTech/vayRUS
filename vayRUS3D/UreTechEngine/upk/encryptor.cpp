#include "encryptor.h"
#include "includes.h"

UreTechEngine::Buffer encryptor::xorEncryptDecrypt(UreTechEngine::Buffer input, const std::string& key)
{
    std::vector<uint8_t> output(input.pointer, input.pointer + input.size);
    size_t keyLength = key.size();

    for (size_t i = 0; i < input.size; ++i) {
        output[i] = *(input.pointer + i) ^ key[i % keyLength];  // Anahtarý döngüsel olarak kullan
    }
    UreTechEngine::Buffer result;
    result.pointer = (uint8_t*)malloc(output.size());
    result.size = output.size();
    memcpy(result.pointer, output.data(), result.size);
    return result;
}