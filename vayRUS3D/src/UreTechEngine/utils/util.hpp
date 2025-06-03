#ifndef util_H
#define util_H

#include<UreTechEngine/utils/dArray.h>

#include<cinttypes>
#include <string>
#include <sstream>

#define CLASS_INHERIT_PUB public : 
#define CLASS_INHERIT_PROT protected :
#define CLASS_INHERIT_PRIV private :

#define CLASS_INHERIT_PUB_ENTITY public : UreTechEngine::entity
#define CLASS_INHERIT_PUB_PLAYER_DO_NOT_USE public : UreTechEngine::Player

namespace UreTechEngine {

    int getBiggestDiv(int divided, int dividing);
    UreTechEngine::string u64ToHexStr(uint64_t value);
    UreTechEngine::string u64ToDecStr(uint64_t value);
    uint64_t strToU64(UreTechEngine::string str);
    UreTechEngine::dArray<UreTechEngine::string> parseWith(UreTechEngine::string str, char c);
    bool isValidPtr(void* ptr);// DONT USE
    void* safePtr(void* ptr);// DONT USE
    struct Buffer {
        uint8_t* pointer = nullptr;
        size_t size = 0;
    };
}

#endif // !util_H