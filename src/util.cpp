#include "util.h"
#include <stdexcept>

int byte_count(uint8_t c) {
    if (c < 128) return 1;
    else if ((c >> 5) == 0b110) return 2;
    else if ((c >> 4) == 0b1110) return 3;
    else if ((c >> 3) == 0b11110) return 4;
    throw std::runtime_error("Invalid UTF-8 byte.");
}

bool is_continuation_byte(uint8_t c) {
    return (c >> 6) == 0b10;
}