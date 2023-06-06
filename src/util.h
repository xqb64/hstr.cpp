#ifndef HSTR_UTIL_H
#define HSTR_UTIL_H

#include <cstdint>
#include <vector>
#include <string>

size_t byte_count(uint8_t c);
bool is_continuation_byte(uint8_t c);
std::vector<std::string> read_file(const char *path);
size_t byte_index(std::string s, size_t pos);
std::pair<std::vector<std::string>::iterator, std::vector<std::string>::iterator> find_range(std::vector<std::string> &vec, size_t n);

#endif