#ifndef util_H
#define util_H
namespace UreTechEngine {

    int getBiggestDiv(int divided, int dividing) {
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
}

#endif // !util_H