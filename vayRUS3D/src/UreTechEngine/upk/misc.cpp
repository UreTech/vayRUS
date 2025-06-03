#include "misc.h"

std::string misc::removeAllSubstrings(std::string input, const std::string& toRemove)
{
    std::string str = input;
    size_t pos;
    while ((pos = str.find(toRemove)) != std::string::npos) {
        str.erase(pos, toRemove.length());
    }
    return str;
}