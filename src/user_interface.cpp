#include <cmath>
#include <sstream>
#include <regex>
#include <rapidfuzz/fuzz.hpp>
#include <unicode/unistr.h>
#include <unicode/locid.h>
#include <unicode/ustream.h>
#include <unicode/schriter.h>
#include <unicode/stsearch.h>
#include "curses.h"
#include "user_interface.h"
#include "util.h"

UserInterface::UserInterface() {
    init_color_pairs();
    read_history();
    search();
}

void UserInterface::search() {
    switch (search_mode) {
        case MODE_EXACT: exact_search(); break;
        case MODE_REGEX: regex_search(); break;
        case MODE_FUZZY: fuzzy_search(); break;
    }
}

void UserInterface::move_highlighted(VerticalDirection d) {
    switch (d) {
        case DIRECTION_UP: {
            if (highlighted > 0) {
                highlighted--;
            } else {
                turn_page(DIRECTION_UP);
                highlighted = entry_count()-1;
            }
            break;
        }
        case DIRECTION_DOWN: {
            if (highlighted < entry_count()-1) {
                highlighted++;
            } else {
                highlighted = 0;
                turn_page(DIRECTION_DOWN);
            }
            break;
        }
    }
    print_history();
}

size_t UserInterface::move_cursor(HorizontalDirection d) {
    size_t old_position = cursor_position;
    switch (d) {
        case DIRECTION_LEFT: {
            if (cursor_position > 0) {
                cursor_position--;
            }
            break;
        }
        case DIRECTION_RIGHT: {
            if (cursor_position < query.char_count()) {
                cursor_position++;
            }
            break;
        }
    }
    reposition_cursor();
    return old_position;
}

void UserInterface::insert_into_query(std::string s) {
    size_t current_idx = byte_index(query, cursor_position);
    query.insert(current_idx, s);
}

void UserInterface::remove_from_query() {
    size_t current_idx = byte_index(query, cursor_position);
    size_t count = byte_count(query[current_idx]);
    query.erase(current_idx, count);
    reposition_cursor();
}

void UserInterface::toggle_search_mode() {
    search_mode = static_cast<SearchMode>((search_mode + 1) % 3);
}

void UserInterface::toggle_case_sensitivity() {
    case_sensitivity = !case_sensitivity;
}

void UserInterface::print_history() {
    clear();

    auto [start, end] = find_range(search_results.empty() ? history : search_results, page);

    for (auto it = start; it != end; it++) {
        size_t idx = std::distance(start, end) - std::distance(it, end);
        bool is_highlighted = idx == highlighted;
        std::string trimmed = trim_string(*it, max_entry_length());

        print(trimmed, idx, 0, is_highlighted ? COLOR_PAIR(3) : COLOR_PAIR(1));

        if (!is_highlighted) {
            paint_matched_chars(*it, idx+1);
        }
    }

    display_status();
    reposition_cursor();
}

void UserInterface::set_error(const char *err) {
    error = err;
}

void UserInterface::exact_search() {
    if (query.empty()) {
        print_history();
        return;
    }

    std::vector<std::string> results;
    std::string _q = case_sensitivity ? query : to_lowercase(query);

    for (std::string entry : history) {
        std::string _e = case_sensitivity ? entry : to_lowercase(entry);

        icu::UnicodeString e(_e.c_str(), "UTF-8");
        icu::UnicodeString q(_q.c_str(), "UTF-8");
        UErrorCode status = U_ZERO_ERROR;

        icu::StringSearch search(q, e, icu::Locale::getRoot(), NULL, status);
        
        int pos = search.first(status);

        if (U_SUCCESS(status) && pos != USEARCH_DONE) {
            results.push_back(entry);
        }
    }

    search_results = std::move(results);
    print_history();
}

void UserInterface::regex_search() {
    std::vector<std::string> results;
    std::string q = case_sensitivity ? query : to_lowercase(query);
 
    for (std::string entry : history) {
        std::string e = case_sensitivity ? entry : to_lowercase(entry);
        try {
            if (std::regex_search(e.cbegin(), e.cend(), std::regex(q))) {
                results.push_back(entry);
                error = nullptr;
            }
        } catch (std::regex_error &err) {
            error = "Invalid regex.";
        }
    }
 
    search_results = std::move(results);
    print_history();
}

void UserInterface::fuzzy_search() {
    if (query.empty()) {
        print_history();
        return;
    }
 
    std::vector<std::string> results;
    std::string q = case_sensitivity ? query : to_lowercase(query);
 
    for (std::string entry : history) {       
        std::string e = case_sensitivity ? entry : to_lowercase(entry);
        if (rapidfuzz::fuzz::partial_ratio(e, q, 65.0)) {
            results.push_back(entry);
        }
    }
 
    search_results = std::move(results);
    print_history();
}

void UserInterface::paint_matched_chars(const std::string &s, size_t row) const {
    std::vector<std::pair<size_t, size_t>> indexes;

    if (search_mode == MODE_FUZZY) {
        if (!case_sensitivity) {
            indexes = find_indexes_fuzzy(to_lowercase(s), to_lowercase(query));
        } else {
            indexes = find_indexes_fuzzy(s, query);
        }
    } else {
        if (!case_sensitivity) {
            indexes = find_indexes(to_lowercase(s), to_lowercase(query));
        } else {
            indexes = find_indexes(s, query);
        }
    }

    for (auto p : indexes) {
        size_t position = find_position(s, p.first);
        print(s.substr(p.first, p.second), row, position, COLOR_PAIR(5) | A_BOLD);
    }
}

void UserInterface::display_status() const {
    if (error) {
        clear();
        display_error();
        return;
    }

    std::ostringstream status;
    status << " page: " 
           << page
           << "/"
           << page_count()
           << " | "
           << "mode: ";

    switch (search_mode) {
        case MODE_EXACT: {
            status << "exact";
            break;
        }
        case MODE_REGEX: {
            status << "regex";
            break;
        }
        case MODE_FUZZY: {
            status << "fuzzy";
            break;
        }
    }

    status << " case: " << (case_sensitivity ? "sensitive" : "insensitive");
    print(status.str(), getmaxy(stdscr)-2, 0, COLOR_PAIR(4));
}

void UserInterface::display_error() const {
    size_t last_row = getmaxy(stdscr)-2;
    print(std::string(error), last_row, 0, COLOR_PAIR(2) | A_BOLD);
}

void UserInterface::print(const std::string &s, size_t row, size_t column, int color_pair) const {
    attron(color_pair);
    mvaddstr(row+1, column+1, s.c_str());
    pad2end();
    attroff(color_pair);
}

void UserInterface::turn_page(VerticalDirection d) {
    clear();
    switch (d) {
        case DIRECTION_UP: {
            if (page > 1) { 
                page--;
            } else {
                page = page_count();
            }
            break;
        }
        case DIRECTION_DOWN: {
            if (page < page_count()) {
                page++;
            } else {
                page = 1;
            }
        }
    }
}

void UserInterface::reposition_cursor() const {
    clear_row(0);
    mvaddstr(0, 1, query.c_str());
    mvaddstr(0, 1, query.substr(0, byte_index(query, cursor_position)).c_str());
}

void UserInterface::read_history() {
    history = read_file("/home/alex/Repositories/hstr/fake_history");
}

size_t UserInterface::page_count() const {
    if (search_results.empty()) return 1;
    return ceil(search_results.size() / static_cast<float>(max_entry_count()));
}

void UserInterface::init_color_pairs() const {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(5, COLOR_RED, COLOR_BLACK);
}

void UserInterface::clear_row(size_t r) const {
    move(r, 0);
    clrtoeol();
}

void UserInterface::pad2end() const {
    int y, x;
    getyx(stdscr, y, x);
    int max_x = getmaxx(stdscr);
    int diff = max_x - x;
    for (int i = 0; i < diff; i++) {
        addch(' ');
    }
}

size_t UserInterface::max_entry_count() const {
    return getmaxy(stdscr) - 2;
}

size_t UserInterface::max_entry_length() const {
    return getmaxx(stdscr) - 2;
}

size_t UserInterface::entry_count() const {
    auto [start, end] = find_range(search_results, page);
    return end - start;
}
