#include "util.h"
#include "cursesw.h"
#include <stdexcept>
#include <fstream>

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
    int rows = getmaxy(stdscr) - 1;  /* number of rows on the screen */
    auto start = vec.begin() + (n-1) * rows;

    int last_n = 0;  /* last n rows after splitting the history equally */
    for ( ; last_n != rows; last_n++) {
        if (start + last_n == vec.end()) break;
    }

    return std::make_pair(start, start + last_n);
}

std::string trim_string(std::string &s, size_t n) {
    size_t idx = byte_index(s, n);
    return s.substr(0, idx);
}
