#ifndef HSTR_USER_INTERFACE_H
#define HSTR_USER_INTERFACE_H

#include <string>
#include <vector>
#include "state.h"

enum Direction {
    DIRECTION_LEFT = -1,
    DIRECTION_UP = -1,
    DIRECTION_RIGHT = 1,
    DIRECTION_DOWN = 1,
};

class UserInterface {
public:
    UserInterface();
    size_t page_count();
    size_t entry_count();
    size_t max_entry_count();
    size_t max_entry_length();
    void turn_page(Direction d);
    void read_history();
    void print_history();
    void move_highlighted(Direction d);
    size_t move_cursor(Direction d);
    void reposition_cursor();
    void insert_into_query(std::string s);
    void remove_from_query();
    void display_error(const char *err);
    void init_color_pairs();
    void clear_row(size_t r);
    void clear_error();
    void pad2end();
private:
    std::vector<std::string> history;
    Query query;
    size_t cursor_position = 0;
    size_t page = 1;
    size_t highlighted = 0;
};

#endif