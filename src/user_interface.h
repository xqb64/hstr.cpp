#ifndef HSTR_USER_INTERFACE_H
#define HSTR_USER_INTERFACE_H

#include <string>
#include <vector>
#include "state.h"

enum SearchMode {
    MODE_EXACT = 0,
    MODE_REGEX = 1,
    MODE_FUZZY = 2,
};

enum VerticalDirection {
    DIRECTION_UP = -1,
    DIRECTION_DOWN = 1,
};

enum HorizontalDirection {
    DIRECTION_LEFT = -1,
    DIRECTION_RIGHT = 1,
};

class UserInterface {
public:
    UserInterface();
    void search();
    void move_highlighted(VerticalDirection d);
    size_t move_cursor(HorizontalDirection d);
    void insert_into_query(std::string s);
    void remove_from_query();
    void toggle_search_mode();
    void toggle_case_sensitivity();
    void print_history();
    void set_error(const char *error);
private:
    void exact_search();
    void regex_search();
    void fuzzy_search();
    void paint_matched_chars(const std::string &s, size_t row);
    void display_status();
    void display_error();
    void print(const std::string &s, size_t row, size_t column, int color_pair);
    void turn_page(VerticalDirection d);
    void read_history();
    void reposition_cursor();
    void init_color_pairs();
    void clear_row(size_t r);
    void pad2end();
    size_t page_count();
    size_t entry_count();
    size_t max_entry_count();
    size_t max_entry_length();
    std::vector<std::string> search_results;
    std::vector<std::string> history;
    Query query;
    size_t cursor_position = 0;
    size_t page = 1;
    size_t highlighted = 0;
    const char *error = NULL;
    SearchMode search_mode = MODE_EXACT;
    bool case_sensitivity = false;
};

#endif