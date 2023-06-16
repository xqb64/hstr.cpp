#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/locid.h>
#include "util.h"

size_t char_count(const std::string &s) {
    size_t total = 0;
    for (size_t i = 0; i < s.length(); i++) {
        if (!is_continuation_byte(s[i])) {
            total++;
        }
    }
    return total;
}

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

std::pair<VecIter, VecIter> find_range(const std::vector<std::string> &vec, size_t page, size_t rows) {
    auto start = vec.cbegin() + (page-1) * rows;

    size_t last_n = 0;  /* last n rows after splitting the history equally */
    for ( ; last_n != rows; last_n++) {
        if (start + last_n == vec.cend()) break;
    }

    return std::make_pair(start, start + last_n);
}

std::string trim_string(const std::string &s, size_t n) {
    size_t idx = byte_index(s, n);
    return s.substr(0, idx);
}

std::optional<std::vector<Index>> find_indexes_fuzzy(const std::string &s, const std::string &q) {
    std::vector<Index> indexes;

    for (size_t i = 0; i < q.length(); ) {
        size_t idx = 0;

        while (true) {
            size_t result = s.find(q.substr(i, byte_count(q[i])), idx);
            if (result == std::string::npos) break;
            indexes.emplace_back(result, byte_count(s[result]));
            idx = result + q.substr(i, byte_count(q[i])).length();
        }

        if (!is_continuation_byte(q[i])) {
            i += byte_count(q[i]);
        }
    }

    return indexes.empty() ? std::nullopt : std::make_optional(indexes);
}

std::optional<std::vector<Index>> find_indexes_exact(const std::string &s, const std::string &q) {
    std::vector<Index> indexes;

    size_t i = 0;
    while (i < s.length()) {
        size_t result = s.find(q, i);
        if (result == std::string::npos) break;
        indexes.emplace_back(result, q.length());
        i = result + byte_count(s[result]);
    }

    return indexes.empty() ? std::nullopt : std::make_optional(indexes);
}

std::optional<std::vector<Index>> find_indexes_regex(const std::string &s, const std::string &q) {
    std::regex r;

    try {
        r = std::regex(q);
    } catch (const std::regex_error &err) {
        return std::nullopt;
    }

    std::vector<Index> indexes;
    for (auto it = std::sregex_iterator(s.begin(), s.end(), r); it != std::sregex_iterator(); it++) {
        indexes.emplace_back(it->position(), it->length());
    }

    return indexes.empty() ? std::nullopt : std::make_optional(indexes);
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

std::string to_lowercase(const std::string &s) {
    icu::UnicodeString us(s.c_str(), "UTF-8");
    std::string tmp;
    us.toLower(icu::Locale::getRoot()).toUTF8String(tmp);
    return tmp;
}

bool compare_lines(const std::string &l1, const std::string &l2, const std::unordered_map<std::string, std::pair<size_t, size_t>> &freqpos_map) {
    const auto &freqpos1 = freqpos_map.at(l1);
    const auto &freqpos2 = freqpos_map.at(l2);

    if (freqpos1.first != freqpos2.first) {
        return freqpos1.first > freqpos2.first;
    } else {
        return freqpos2.second > freqpos1.second;
    }
}

void sort_lines(std::vector<std::string> &v) {
    std::unordered_map<std::string, std::pair<size_t, size_t>> freqpos_map;

    for (size_t i = 0; i < v.size(); i++) {
        freqpos_map[v[i]].first++;
        freqpos_map[v[i]].second = i;
    }

    std::sort(v.begin(), v.end(), [&freqpos_map](const std::string &l1, const std::string &l2){
        return compare_lines(l1, l2, freqpos_map);       
    });
}

void deduplicate_lines(std::vector<std::string> &v) {
    std::unordered_set<std::string> unique_lines;
    v.erase(std::remove_if(v.begin(), v.end(), [&unique_lines](const std::string &l) {
        return !unique_lines.insert(l).second;
    }), v.end());
}
