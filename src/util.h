#ifndef HSTR_UTIL_H
#define HSTR_UTIL_H

#include <cstdint>
#include <vector>
#include <string>

typedef std::vector<std::string>::const_iterator VecIter;
typedef std::pair<size_t, size_t> Index;

size_t byte_count(uint8_t c);
bool is_continuation_byte(uint8_t c);
std::vector<std::string> read_file(const char *path);
size_t byte_index(std::string s, size_t pos);
std::pair<VecIter, VecIter> find_range(const std::vector<std::string> &vec, size_t n);
std::string trim_string(const std::string &s, size_t n);
std::vector<Index> find_indexes(const std::string &s, const std::string &q);
std::vector<Index> find_indexes_fuzzy(const std::string &s, const std::string &q);
std::size_t find_position(const std::string &s, size_t idx);
std::string to_lowercase(const std::string &s);

#endif