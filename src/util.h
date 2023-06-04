#ifndef HSTR_UTIL_H
#define HSTR_UTIL_H

#include <cstdint>

int byte_count(uint8_t c);
bool is_continuation_byte(uint8_t c);

#endif