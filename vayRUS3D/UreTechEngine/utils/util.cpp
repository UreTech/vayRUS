#include "util.hpp"

int UreTechEngine::getBiggestDiv(int divided, int dividing)
{
    int remain = divided;
    int result = 0;
    if (divided < 0) {
        return 0;
    }
    while (remain > 0) {
        remain -= dividing;
        result++;
    }
    result--;

    return result;
}

std::string UreTechEngine::intToHex(int value)
{
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << value;
    return ss.str();
}

//for debug bugs

bool UreTechEngine::isValidPtr(void* ptr)
{
#ifdef _DEBUG
    return !(ptr == nullptr || ptr == (void*)0xcdcdcdcdcdcdcdcd);
#else
    return ptr != nullptr;
#endif // _DEBUG
}

void* UreTechEngine::safePtr(void* ptr)
{
#ifdef _DEBUG
    if (ptr == (void*)0xcdcdcdcdcdcdcdcd) {
        return nullptr;
    }
    else {
        return ptr;
    }
#else
    return ptr;
#endif // _DEBUG
}

