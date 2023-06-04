#include "curses.h"
#include "user_interface.h"
#include "util.h"

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

void UserInterface::reposition_cursor() {
    clear_row(0);
    mvaddstr(0, 1, query.c_str());
    mvaddstr(0, 1, query.substr(0, query.byte_index(cursor_position)).c_str());
}

void UserInterface::insert_into_query(std::string s) {
    size_t current_idx = query.byte_index(cursor_position);
    query.insert(current_idx, s);
}

void UserInterface::remove_from_query() {
    size_t current_idx = query.byte_index(cursor_position);
    int count = byte_count(query[current_idx]);
    query.erase(current_idx, count);
    reposition_cursor();
}

void UserInterface::clear_row(size_t r) {
    move(r, 0);
    clrtoeol();
}