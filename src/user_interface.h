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
    void move_cursor(CursorDirection d);
    void reposition_cursor();
    void insert_into_query(std::string s);
    void remove_from_query();
    void clear_row(size_t r);
private:
    Query query;
    size_t cursor_position = 0;
};

#endif