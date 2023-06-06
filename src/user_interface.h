#ifndef HSTR_USER_INTERFACE_H
#define HSTR_USER_INTERFACE_H

#include <string>
#include <vector>
#include "state.h"

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
    void display_error(const char *err);
    void clear_error();
    void print_history();
private:
    size_t page_count();
    size_t entry_count();
    size_t max_entry_count();
    size_t max_entry_length();
    void display_status();
    void turn_page(VerticalDirection d);
    void read_history();
    void reposition_cursor();
    void init_color_pairs();
    void clear_row(size_t r);
    void pad2end();
    std::vector<std::string> search_results;
    std::vector<std::string> history;
    Query query;
    size_t cursor_position = 0;
    size_t page = 1;
    size_t highlighted = 0;
};

#endif