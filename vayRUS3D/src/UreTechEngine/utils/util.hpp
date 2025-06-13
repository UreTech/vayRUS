#ifndef util_H
#define util_H

#include<UreTechEngine/utils/dArray.h>

#include<cinttypes>
#include <string>
#include <sstream>
#include <chrono>

#define CLASS_INHERIT_PUB public : 
#define CLASS_INHERIT_PROT protected :
#define CLASS_INHERIT_PRIV private :

#define CLASS_INHERIT_PUB_ENTITY public : UreTechEngine::entity
#define CLASS_INHERIT_PUB_PLAYER_DO_NOT_USE public : UreTechEngine::Player

namespace UreTechEngine {

    int ENGINE_DEFINE getBiggestDiv(int divided, int dividing);
    UreTechEngine::string ENGINE_DEFINE u64ToHexStr(uint64_t value);
    UreTechEngine::string ENGINE_DEFINE u64ToDecStr(uint64_t value);
    uint64_t ENGINE_DEFINE strToU64(UreTechEngine::string str);
    UreTechEngine::dArray<UreTechEngine::string> ENGINE_DEFINE parseWith(UreTechEngine::string str, char c);
    bool isValidPtr(void* ptr);// DONT USE
    void* safePtr(void* ptr);// DONT USE
    struct Buffer {
        uint8_t* pointer = nullptr;
        size_t size = 0;
    };

    class ENGINE_DEFINE timer {
    public:
        timer();
        timer(size_t ms);
        void set_timer_ms(size_t ms);
		void reset();
		size_t get_elapsed_ms();
		void start();
		bool async_wait();
    private:
		std::chrono::steady_clock::time_point start_time;
		size_t timer_ms = 0;
    };
}

#endif // !util_H