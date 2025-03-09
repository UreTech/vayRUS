#ifndef util_H
#define util_H

#include<cinttypes>
#include <string>
#include <sstream>

namespace UreTechEngine {

    int getBiggestDiv(int divided, int dividing);
    std::string intToHex(int value);
    bool isValidPtr(void* ptr);// DONT USE
    void* safePtr(void* ptr);// DONT USE
    struct Buffer {
        uint8_t* pointer = nullptr;
        size_t size = 0;
    };
}

#endif // !util_H