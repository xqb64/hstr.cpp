#include <cmath>
#include <sstream>
#include <regex>
#include <rapidfuzz/fuzz.hpp>
#include "curses.h"
#include "user_interface.h"
#include "util.h"

UserInterface::UserInterface() {
    init_color_pairs();
    read_history();
    search();
}

void UserInterface::read_history() {
    history = read_file("/home/alex/Repositories/hstr/fake_history");
}

void UserInterface::toggle_search_mode() {
    search_mode = static_cast<SearchMode>((search_mode + 1) % 3);
    search();
    display_status();
}

void UserInterface::search() {
    switch (search_mode) {
        case MODE_EXACT: exact_search(); break;
        case MODE_REGEX: regex_search(); break;
        case MODE_FUZZY: fuzzy_search(); break;
    }
}

void UserInterface::exact_search() {
    std::vector<std::string> results;
    for (auto it = history.cbegin(); it != history.cend(); it++) {
        if (it->find(query) != std::string::npos) {
            results.push_back(*it);
        }
    }
    search_results = std::move(results);
    print_history();
}

void UserInterface::fuzzy_search() {
    std::vector<std::string> results;
    for (auto it = history.cbegin(); it != history.cend(); it++) {
        if (rapidfuzz::fuzz::partial_ratio(*it, query, 65.0)) {
            results.push_back(*it);
        }
    }
    search_results = std::move(results);
    print_history();
}

void UserInterface::regex_search() {
    std::vector<std::string> results;
    for (auto it = history.cbegin(); it != history.cend(); it++) {
        try {
            if (std::regex_search(it->cbegin(), it->cend(), std::regex(query))) {
                results.push_back(*it);
                error = nullptr;
            }
        } catch (std::regex_error &e) {
            error = "Invalid regex.";
        }
    }
    search_results = std::move(results);
    print_history();
}

void UserInterface::print_history() {
    clear();
    auto [start, end] = find_range(search_results, page);
    for (auto it = start; it != end; it++) {
        size_t idx = std::distance(start, end) - std::distance(it, end);
        bool is_highlighted = idx == highlighted;
        std::string trimmed = trim_string(*it, max_entry_length());
        
        if (is_highlighted) {
            attron(COLOR_PAIR(3));
            mvaddstr(idx+1, 1, trimmed.c_str());
            pad2end();
            attroff(COLOR_PAIR(3));
        } else {
            attron(COLOR_PAIR(1));
            mvaddstr(idx+1, 1, trimmed.c_str());
            pad2end();
            paint_matched_chars(*it, idx+1);
            attroff(COLOR_PAIR(1));
        }
    }
    display_status();
    reposition_cursor();
}

void UserInterface::display_status() {
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
    attron(COLOR_PAIR(4));
    mvaddstr(getmaxy(stdscr)-1, 1, status.str().c_str());
    pad2end();
    attroff(COLOR_PAIR(4));
}

size_t UserInterface::page_count() {
    return ceil(search_results.size() / static_cast<float>(max_entry_count()));
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

size_t UserInterface::max_entry_count() {
    return getmaxy(stdscr) - 2;
}

size_t UserInterface::max_entry_length() {
    return getmaxx(stdscr) - 2;
}

size_t UserInterface::entry_count() {
    auto [start, end] = find_range(search_results, page);
    return end - start;
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

void UserInterface::paint_matched_chars(const std::string &s, size_t row) {
    std::vector<std::pair<size_t, size_t>> indexes;
    if (search_mode == MODE_FUZZY) {
        indexes = find_indexes_fuzzy(s, query);
    } else {
        indexes = find_indexes(s, query);
    }
    for (auto p : indexes) {
        size_t position = find_position(s, p.first);
        attron(COLOR_PAIR(5) | A_BOLD);
        mvaddstr(row, position+1, s.substr(p.first, p.second).c_str());
        attroff(COLOR_PAIR(5) | A_BOLD);
    }
}

void UserInterface::reposition_cursor() {
    clear_row(0);
    mvaddstr(0, 1, query.c_str());
    mvaddstr(0, 1, query.substr(0, byte_index(query, cursor_position)).c_str());
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

void UserInterface::display_error() {
    size_t last_row = getmaxy(stdscr)-1;
    attron(COLOR_PAIR(2) | A_BOLD);
    mvaddstr(last_row, 1, error);
    pad2end();
    attroff(COLOR_PAIR(2) | A_BOLD);
}

void UserInterface::init_color_pairs() {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(5, COLOR_RED, COLOR_BLACK);
}

void UserInterface::clear_row(size_t r) {
    move(r, 0);
    clrtoeol();
}

void UserInterface::pad2end() {
    int y, x;
    getyx(stdscr, y, x);
    int max_x = getmaxx(stdscr);
    int diff = max_x - x;
    for (int i = 0; i < diff; i++) {
        addch(' ');
    }
}

void UserInterface::set_error(const char *err) {
    error = err;
}