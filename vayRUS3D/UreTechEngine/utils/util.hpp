#ifndef util_H
#define util_H

#include<cinttypes>

namespace UreTechEngine {

    int getBiggestDiv(int divided, int dividing);

    struct Buffer {
        uint8_t* pointer = nullptr;
        size_t size = 0;
    };
}

#endif // !util_H