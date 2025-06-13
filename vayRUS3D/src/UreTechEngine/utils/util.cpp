#include "util.hpp"
#include "dArray.h"

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

UreTechEngine::string UreTechEngine::u64ToHexStr(uint64_t value)
{
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << value;
    return string(ss.str().data(), ss.str().size());
}

UreTechEngine::string UreTechEngine::u64ToDecStr(uint64_t value)
{
    return string::stdStrToUStr(std::to_string(value));
}

uint64_t UreTechEngine::strToU64(UreTechEngine::string str)
{
    return std::stoull(str.std_str());
}

UreTechEngine::dArray<UreTechEngine::string> UreTechEngine::parseWith(UreTechEngine::string str, char c)
{
    UreTechEngine::dArray<UreTechEngine::string> res;
    UreTechEngine::string block;
    for (uint64_t i = 0; i < str.lenght(); i++) {
        if (str[i] != c) {
            block.push_back(str[i]);
        }
        else {
            res.push_back(block);
            block = "";
        }
    }
    res.push_back(block);
    return res;
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

UreTechEngine::timer::timer()
{
    this->start_time = std::chrono::high_resolution_clock::now();
    this->timer_ms = 0;
}

UreTechEngine::timer::timer(size_t ms)
{
    this->start_time = std::chrono::high_resolution_clock::now();
    this->timer_ms = ms;
}

void UreTechEngine::timer::set_timer_ms(size_t ms)
{
	this->timer_ms = ms;
}

void UreTechEngine::timer::reset()
{
	this->start_time = std::chrono::high_resolution_clock::now();
}

size_t UreTechEngine::timer::get_elapsed_ms()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - this->start_time).count();
}

void UreTechEngine::timer::start()
{
	this->reset();
}

bool UreTechEngine::timer::async_wait()
{
    return this->get_elapsed_ms() >= this->timer_ms;
}
