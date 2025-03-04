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

