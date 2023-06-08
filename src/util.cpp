#include "util.h"
#include "cursesw.h"
#include <stdexcept>
#include <fstream>
#include <regex>

size_t byte_count(uint8_t c) {
    if (c < 128) return 1;
    else if ((c >> 5) == 0b110) return 2;
    else if ((c >> 4) == 0b1110) return 3;
    else if ((c >> 3) == 0b11110) return 4;
    throw std::runtime_error("Invalid UTF-8 byte.");
}

bool is_continuation_byte(uint8_t c) {
    return (c >> 6) == 0b10;
}

std::vector<std::string> read_file(const char *path) {
    std::vector<std::string> content;
    std::ifstream stream(path);
    std::string line;
    while (std::getline(stream, line)) {
        content.push_back(line);
    }
    return content;
}

size_t byte_index(std::string s, size_t pos) {
    size_t idx = 0;
    size_t current_char = 0;
    while (idx < s.length()) {
        if (current_char == pos) break;
        if (!is_continuation_byte(s[idx])) {
            current_char++;
        }
        idx += byte_count(s[idx]);
    }
    return idx;
}


std::pair<std::vector<std::string>::iterator, std::vector<std::string>::iterator> find_range(std::vector<std::string> &vec, size_t n) {
    int rows = getmaxy(stdscr) - 2;  /* number of rows on the screen */
    auto start = vec.begin() + (n-1) * rows;

    int last_n = 0;  /* last n rows after splitting the history equally */
    for ( ; last_n != rows; last_n++) {
        if (start + last_n == vec.end()) break;
    }

    return std::make_pair(start, start + last_n);
}

std::string trim_string(const std::string &s, size_t n) {
    size_t idx = byte_index(s, n);
    return s.substr(0, idx);
}

std::vector<std::pair<size_t, size_t>> find_indexes_fuzzy(const std::string &s, const std::string &q) {
    std::vector<std::pair<size_t, size_t>> indexes;
    for (size_t i = 0; i < q.length(); ) {
        size_t idx = 0;
        while (true) {
            size_t result = s.find(q.substr(i, byte_count(q[i])), idx);
            if (result == std::string::npos) {
                break;
            }
            indexes.push_back(std::make_pair(result, byte_count(s[result])));
            idx = result+q.substr(i, byte_count(q[i])).length();
        }
        if (!is_continuation_byte(q[i])) {
            i += byte_count(q[i]);
        }
    }
    return indexes;
}

std::vector<std::pair<size_t, size_t>> find_indexes(const std::string &s, const std::string &q) {
    if (q.empty()) return {};
    std::vector<std::pair<size_t, size_t>> indexes;
    std::regex r(q);
    for (auto it = std::sregex_iterator(s.begin(), s.end(), r); it != std::sregex_iterator(); it++) {
        indexes.push_back(std::make_pair(it->position(), it->length()));
    }
    return indexes;
}

std::size_t find_position(const std::string &s, size_t idx) {
    size_t i = 0;
    size_t position = 0;
    while (i != idx) {
        if (!is_continuation_byte(s[i])) {
            position++;
        }
        i += byte_count(s[i]);
    }
    return position;
}