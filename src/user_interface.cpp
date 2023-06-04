#include "curses.h"
#include "user_interface.h"

void UserInterface::move_cursor(CursorDirection d) {
    switch (d) {
        case CURSOR_LEFT: {
            if (cursor_position > 0) {
                cursor_position--;
            }
            break;
        }
        case CURSOR_RIGHT: {
            if (cursor_position < query.character_count()) {
                cursor_position++;
            }
            break;
        }
    }
    reposition_cursor();
}

void UserInterface::insert_into_query(std::string s) {
    query.insert(query.byte_index(cursor_position), s);
}

void UserInterface::reposition_cursor() {
    clear_row(0);
    mvaddstr(0, 1, query.c_str());
    mvaddstr(0, 1, query.substr(0, query.byte_index(cursor_position)).c_str());
}

void UserInterface::clear_row(size_t r) {
    move(r, 0);
    clrtoeol();
}