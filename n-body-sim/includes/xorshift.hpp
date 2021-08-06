#pragma once
#include <cstdint>

namespace n_body {

class Xorshift32 {
public:
    Xorshift32(uint32_t seed);
    uint32_t get_rand();
    float rand_float();
private:
    uint32_t cur_state;
};

}
