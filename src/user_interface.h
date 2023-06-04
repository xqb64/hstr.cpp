#ifndef HSTR_USER_INTERFACE_H
#define HSTR_USER_INTERFACE_H

#include <string>
#include "state.h"

enum CursorDirection {
    CURSOR_LEFT = -1,
    CURSOR_RIGHT = 1,
};

class UserInterface {
public:
    UserInterface();
    void move_cursor(CursorDirection d);
    void reposition_cursor();
    void insert_into_query(std::string s);
    void remove_from_query();
    void display_error(const char *err);
    void init_color_pairs();
    void clear_row(size_t r);
    void clear_error();
    void pad2end();
private:
    Query query;
    size_t cursor_position = 0;
};

#endif